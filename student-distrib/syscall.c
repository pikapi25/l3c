#include "syscall.h"

uint32_t pid_arr[MAX_PROCESSES] = {0};

/* get_pid
 * INPUT: none
 * OUTPUT: current pid
 * Effect: none
*/
uint32_t get_pid(){
    uint32_t esp_value;
    /* get the esp value */
    asm volatile ("movl %%esp, %0" : "=r" (esp_value));
    /* current pid is (8MB-eps)/8KB */
    return ((PCB_START - esp_value)/PCB_BLOCK_SIZE);
}
/* get_pcb
 * INPUT: the corresponding pid to find pcb
 * OUTPUT: a pointer to pcb
 * Effect: none
*/
pcb_t* get_pcb(uint32_t pid){
    /* i-th PCB struct starts at 8MB – (i+1) * 8KB */
    return (pcb_t*)(PCB_START - PCB_BLOCK_SIZE*(pid+1));
}


