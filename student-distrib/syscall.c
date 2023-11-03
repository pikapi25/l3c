#include "syscall.h"
#include "x86_desc.h"
#include "paging.h"
#include "lib.h"

uint32_t pid_arr[MAX_PROCESSES] = {0};

/* get_pcb
 * INPUT: none
 * OUTPUT: a pointer to pcb
 * Effect: none
*/
pcb_t* get_cur_pcb(){
    uint32_t esp_value;
    /* get the esp value */
    asm volatile ("movl %%esp, %0" : "=r" (esp_value));

    return (pcb_t*)(esp_value & PCB_BITMASK);
}

file_op_t terminal_ops = {illegal_open, illegal_close, terminal_read, terminal_write};
file_op_t rtc_ops = {rtc_open, rtc_close, rtc_read, rtc_write};
file_op_t dir_ops = {open_dir, close_dir, read_dir, write_dir};
file_op_t file_ops = {open_file, close_file, read_file, write_file};
/* halt
 * INPUT: the status value
 * OUTPUT: return a value to the parent execute system call so that we know how the program ended.
 * Effect: none
*/
int32_t halt (uint8_t status){
    int i;
    uint32_t ret_val = (uint32_t)status;
    /* get current pcb */
    pcb_t *cur_pcb = get_cur_pcb();
    if(cur_pcb == NULL){
        return -1;
    }

    /* close all files */
    for (i=0; i < MAX_FILES; i++){
        if (cur_pcb->fd_arr[i].flags!=0){
            //cur_pcb->fd_arr[i].file_op_table->close_op(i);
            cur_pcb->fd_arr[i].flags = 0;
        }
    }

    /* get parent pcb */
    pcb_t *parent_pcb = cur_pcb->parent_pcb;

    /* remove current pcb */
    pid_arr[cur_pcb->pid] = 0;

    /* if it's shell, close the shell and start a new one */
    /* ATTENTION: Need to set parent_pcb to NULL when initializing*/
    if (parent_pcb == NULL){
        pid_arr[cur_pcb->pid] = 0;
        execute((uint8_t*)"shell");
    }

    /* Restore parent paging (from ls back to shell) */
    /* Physical memory starts at 8MB + (process number * 4MB) */
    /* ATTENTION: Need to implement this function to set paging */
    mapping_vir_to_phy(VIRTUAL_PAGE_START, PCB_BOTTOM+(parent_pcb->pid)*PHYS_PROGRAM_SIZE);                                   \
    /* Write Parent process’ info back to TSS(esp0) */
    /* the esp should point to the bottom of the parent block after halting current pcb */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = PCB_BOTTOM - (parent_pcb->pid)*PCB_BLOCK_SIZE - sizeof(int32_t);

    /* Jump to execute return */
    /* ATTENTION: Neet to implement this function to restore ebp and esp value in asm*/

	asm volatile("mov %0, %%esp \n\
                mov %1, %%ebp \n\
                mov %2, %%eax \n\
                leave         \n\
                ret            \n\
               "
               :
               : "r"(cur_pcb->esp_val), "r" (cur_pcb->ebp_val), "r"(ret_val)
               : "%eax", "ebp","esp");
    return 0;
}
//              jmp BACK_TO_EXECUTE \n
/* execute
 * INPUT: cmd: spaced-separated sequences of words
        [file name of the program to be executed] [arg1, arg2, arg3]
 * RETURN: 
 * -1 if the command cannot be exexuted 
 * (eg: the program doen not exist/the filename is not executable)
 * 256 if the program dies by an exception
 * 0-255 if the program executes a halt system call, in which case
 * the value returned is that given by the program’s call to halt
 */
int32_t execute(const uint8_t* command)
{
    int result;
    uint32_t i, prog_start_addr, pid = 0;
    uint8_t eip[4];
    uint8_t file_name[FILENAME_LEN] = {'\0'}; 
    // char args[MAX_CHA_BUF + 1] = {'\0'};    //contains " "
    
    /* basic validation check */
    // if(curr_pid >= MAX_PROCESSES) 
    //     return FAILURE;
    if(command == NULL||command == '\0')
        return FAILURE;

    /* Parse cmd */
    // uint16_t cmd_len = strlen((int8_t*)command);
    // for(i=0;i<cmd_len;i++){
    //     if(command[i] == ' ') break;
    //     file_name[i] = command[i];
    // }
    i=0;
    while(command[i]!=' ' && command[i]!='\0'){
        file_name[i] = command[i];
        i++;
    }
    // /* Parse args */
    // int arg_idx = i;
    // for(i=0;arg_idx<cmd_len;arg_idx++){
    //     if(command[arg_idx] == '\0') break;
    //     args[i] = command[arg_idx];
    //     i++;
    // }
    
    dentry_t dentry;
    uint8_t mag_buf[4];
    /* check if file is valid*/
    if(read_dentry_by_name((uint8_t*)file_name, &dentry) == FAILURE)
        return FAILURE;

    /* check if we can read first several bytes from the file */
    if(4 != read_data( dentry.inode_num, 0, mag_buf, 4)) return FAILURE;

    /* check if it is executeable: ELF's magic number = 0x7f454c46 */ 
    if((mag_buf[0] != MAGIC_NUM_0) || (mag_buf[1] != MAGIC_NUM_1) ||(mag_buf[2] != MAGIC_NUM_2) ||(mag_buf[3] != MAGIC_NUM_3))
        return FAILURE;
    
    /* get eip */
    // result = read_data(dentry.inode_num, 24, eip, 4);
    // if(result != 4) return FAILURE;   
    // prog_start_addr = (eip[0]<<24)+(eip[1]<<16)+(eip[2]<<8)+eip[3];
    result = read_data(dentry.inode_num, 24, (uint8_t* )&prog_start_addr, 4);
    if(result != 4) return FAILURE;
    
    /* get a new PID for the new process */
    pid = 0;
    while(pid_arr[pid]!=0 && pid<MAX_PROCESSES){
        pid++;
    }
    /* If no PIDs are free, return FAILURE. */
    if(pid == MAX_PROCESSES) return FAILURE;
    /* get pcb structture */
    pcb_t* pcb = (pcb_t*)(EIGHT_MB - (pid+1) * EIGHT_KB);
    pid_arr[pid] = 1;
    pcb->pid = pid;

    /* set parent pid */
    if(pid == 0){
        pcb->parent_pcb =(pcb_t*)( EIGHT_MB - EIGHT_KB);  
    }
    else{
        pcb->parent_pcb = (pcb_t*)(EIGHT_MB - pid * EIGHT_KB);
    }

    pcb->fd_arr[0].flags = 1;
    pcb->fd_arr[0].inode = 0;
    pcb->fd_arr[0].file_position = 0;
    pcb->fd_arr[0].file_op_table = &terminal_ops;
    
    pcb->fd_arr[1].flags = 1;
    pcb->fd_arr[1].inode = 0;
    pcb->fd_arr[1].file_position = 0;
    pcb->fd_arr[1].file_op_table = &terminal_ops;

    /* Set up paging */
    mapping_vir_to_phy(VIRTUAL_PAGE_START, PCB_BOTTOM+(pcb->pid)*PHYS_PROGRAM_SIZE);

    /* Load program */
    read_data(dentry.inode_num, 0, (uint8_t*)USER_CODE, USER_STACK - USER_CODE);

    /* Save old stack */
    uint32_t saved_ebp, saved_esp;
    asm("movl %%ebp, %0" : "=r"(saved_ebp));
    asm("movl %%esp, %0" : "=r"(saved_esp));
    // PCB represents our source
    pcb->ebp_val = saved_ebp;                 // so that we can return to the parent
    pcb->esp_val = saved_esp;
    // TSS represents our destination
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - pid * EIGHT_KB - 4;    // the child pid about to be created

    /* Enter user mode */
    // Push order: SS, ESP, EFLAGS, CS, EIP
    // ESP points to the base of user stack (132MB - 4B)
  asm volatile("mov $0x2B, %%ax;"
                "mov %%ax, %%ds;"
                "mov %%ax, %%es;"
                "mov %%ax, %%fs;"
                "mov %%ax, %%gs;"
                "pushl $0x2B;"
                //132MB - 1 bottom of user page
                "pushl $0x83FFFFC;"
                "pushfl;"
                "popl %%edx;"
                //OR the IF bit (10th bit) of flags to set to 1 because I cli()ed it
                "orl $0x200, %%edx;"
                "pushl %%edx;"
                //push user code segment
                "pushl $0x23;"
                //push entry point
                "pushl %0;"
                "iret;"
                "BACK_TO_EXECUTE: "
               :
               : "r"(prog_start_addr)
               : "edx", "eax"
               );
    return 0;
}


/* open
 * INPUT: filename
 * OUTPUT: allocated fd. or -1 on failure
 * Effect: allocate avaliable fd.
*/
int32_t open(const uint8_t* filename){
    /* 1. find file */
    int i;
    if (open_file(filename) == -1) return -1;

    /* 2. allocate an unused file descriptor */
    pcb_t * cur_pcb = get_cur_pcb();
    for (i = 0; i < MAX_FILES; i++){
        if (cur_pcb->fd_arr[i].flags == Free){
            break;
        }
    }
    if (i == MAX_FILES) return FAILURE;

    /* 3. set up descriptor*/
    dentry_t dentry;
    read_dentry_by_name(filename, &dentry);
    cur_pcb->fd_arr[i].file_position = 0;
    cur_pcb->fd_arr[i].flags = Busy;
    cur_pcb->fd_arr[i].inode = dentry.inode_num;

    if (dentry.filetype == 0){
        /* rtc type */
        cur_pcb->fd_arr[i].file_op_table = &rtc_ops;
    }else if (dentry.filetype == 1){
        /* directory type */
        cur_pcb->fd_arr[i].file_op_table = &dir_ops;
    }else if (dentry.filetype == 2){
        /* data file type */
        cur_pcb->fd_arr[i].file_op_table = &file_ops;
    }
    cur_pcb->fd_arr[i].file_op_table->open_op(filename);
    return i;
}

/* close
 * INPUT: fd
 * OUTPUT: 0 on success; -1 on tyring close invalid fd
 * Effect: close one fd.
*/
int32_t close(int32_t fd){
    /* close default fd and invalid fd is forbidden */
    if (fd <= 1 || fd >= MAX_FILES)return -1;

    /* close free fd is forbidden*/
    pcb_t * cur_pcb = get_cur_pcb();
    if (cur_pcb->fd_arr[fd].flags == Busy){
        cur_pcb->fd_arr[fd].file_op_table->close_op(fd);
        cur_pcb->fd_arr[fd].flags = Free;
        return 0;
    }
    return -1;
}

/* read
 * INPUT: fd, buf, nbytes
 * OUTPUT: bytes read; -1 on failure
 * Effect: read call is executed.
*/
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    int32_t result;
    pcb_t * cur_pcb = get_cur_pcb();
    if (fd < 0 || fd > MAX_FILES)return -1;
    if (buf == NULL || nbytes < 0)return -1;
    if (cur_pcb->fd_arr[fd].flags == Free)return -1;

    /* use file operations jump table to call read function*/
    result = cur_pcb->fd_arr[fd].file_op_table->read_op(fd, buf, nbytes);
    return result;
}

/* write
 * INPUT: fd, buf, nbytes
 * OUTPUT: bytes written; -1 on failure
 * Effect: read call is executed.
*/
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    int32_t result;
    pcb_t * cur_pcb = get_cur_pcb();
    if (fd < 0 || fd > MAX_FILES)return -1;
    if (buf == NULL || nbytes < 0)return -1;
    if (cur_pcb->fd_arr[fd].flags == Free)return -1;

    /* use file operations jump table to call write function*/
    result = cur_pcb->fd_arr[fd].file_op_table->write_op(fd, buf, nbytes);
    return result;
}

