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
    return 0;
}


