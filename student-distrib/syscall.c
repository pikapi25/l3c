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


