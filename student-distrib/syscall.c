#include "syscall.h"

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
    uint32_t ret;
    int i;
    ret = (uint32_t)status;

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

    /* if it's shell, close the shell and start a new one */
    if (parent_pcb == NULL){
        pid_arr[cur_pcb->pid] = 0;
        execute((uint8_t*)"shell");
    }
    
    return 0;
}


