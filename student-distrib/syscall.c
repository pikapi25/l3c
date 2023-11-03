#include "syscall.h"
#include "x86_desc.h"
#include "paging.h"

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


/* halt
 * INPUT: the status value
 * OUTPUT: return a value to the parent execute system call so that we know how the program ended.
 * Effect: none
*/
int32_t halt (uint8_t status){
    int i;

    /* get current pcb */
    pcb_t *cur_pcb = get_cur_pcb();
    if(cur_pcb == NULL){
        return -1;
    }

    /* close all files */
    for (i=0; i < MAX_FILES; i++){
        if (cur_pcb->fd_arr[i].flags!=0){
            cur_pcb->fd_arr[i].file_op_table->close_op(i);
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
    mapping_vir_to_phy(VIRTUAL_PAGE_START, PCB_BOTTOM+(parent_pcb->pid)*PHYS_PROGRAM_SIZE);

    /* Write Parent process’ info back to TSS(esp0) */
    /* the esp should point to the bottom of the parent block after halting current pcb */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = PCB_BOTTOM - (parent_pcb->pid)*PCB_BLOCK_SIZE - sizeof(int32_t);

    /* Jump to execute return */
    /* ATTENTION: Neet to implement this function to restore ebp and esp value in asm*/
    halt_jmp_to_exe_ret(cur_pcb->ebp_val, cur_pcb->esp_val, status);
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
    if (i = MAX_FILES)return -1;

    /* 3. set up descriptor*/
    dentry_t dentry;
    read_dentry_by_name(filename, &dentry);
    cur_pcb->fd_arr[i].file_position = 0;
    cur_pcb->fd_arr[i].flags = Busy;
    cur_pcb->fd_arr[i].inode = dentry.inode_num;

    if (dentry.filetype == 0){
        /* rtc type */
        cur_pcb->fd_arr[i].file_op_table->close_op = rtc_close;
        cur_pcb->fd_arr[i].file_op_table->open_op = rtc_open;
        cur_pcb->fd_arr[i].file_op_table->read_op = rtc_read;
        cur_pcb->fd_arr[i].file_op_table->write_op = rtc_write;
    }else if (dentry.filetype == 1){
        /* directory type */
        cur_pcb->fd_arr[i].file_op_table->close_op = close_dir;
        cur_pcb->fd_arr[i].file_op_table->open_op = open_dir;
        cur_pcb->fd_arr[i].file_op_table->read_op = read_dir;
        cur_pcb->fd_arr[i].file_op_table->write_op = write_dir;
    }else if (dentry.filetype == 2){
        /* data file type */
        cur_pcb->fd_arr[i].file_op_table->close_op = close_file;
        cur_pcb->fd_arr[i].file_op_table->open_op = open_file;
        cur_pcb->fd_arr[i].file_op_table->read_op = read_file;
        cur_pcb->fd_arr[i].file_op_table->write_op = write_file;
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

