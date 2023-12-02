#include "scheduler.h" 
#include "syscall.h"
#include "i8259.h"
#define video_memory_start 0xb8000


/**
 * PIT_init
 * 
 * Description: Initializes the Programmable Interval Timer (PIT) by setting mode/command register,
 *              setting the frequency to channel 0 data port, and enabling interrupts.
 * 
 * Inputs: None
 * Return value: None
 * Side effects: None.
 */
void pit_init()
{
    //set the value of PIT mode/command register
    outb(PIT_REG, PIT_IO_PORT);

    //set frequency to channel 0 data port
    outb(PIT_FREQ & LOWER_MASK, CHANNEL_0);
    outb(PIT_FREQ >> 8, CHANNEL_0);     //right shift 8 to get the high byte

    //enable interrupts
    enable_irq(PIT_IRQ);
}


/**
 * pit_handler
 * 
 * Description: Interrupt handler for the Programmable Interval Timer (PIT).
 *              Performs task scheduling and sends an EOI to the PIC.
 * 
 * Inputs: None
 * Return value: None
 * Side effects: May modify task scheduling and acknowledge interrupt to PIC.
 */
void pit_handler(){
    cli();               
    scheduler();                 
    send_eoi(PIT_IRQ);          //Send EOI to PIC
    sti();                 
}

/**
 * scheduler_initialize
 * 
 * Description: Initialize scheduler 
 * 
 * Inputs: None
 * Return value: none
 * Side effects: change scheduler 
 */
void scheduler_initialize(){
    myScheduler.cur_task = 0;
    myScheduler.num_tasks = 0;
    myScheduler.tasks[0]=NOT_EXIST;
    myScheduler.tasks[1]=NOT_EXIST;
    myScheduler.tasks[2]=NOT_EXIST;
}

/**
 * scheduler
 * 
 * Description: change to next process
 * 
 * Inputs: None
 * Return value: none
 * Side effects: change scheduler
*/
void scheduler(){
    int32_t current_pointer = myScheduler.cur_task;        // pointer of current process
    uint32_t current_pid = myScheduler.tasks[current_pointer];
    int32_t next_pointer = scheduler_getnext();
    uint32_t next_pid  = myScheduler.tasks[next_pointer];
    
    pcb_t* current_pcb = get_cur_pcb();
    register uint32_t saved_ebp asm("ebp");
    current_pcb->sch_ebp = saved_ebp;

    if (next_pid < 0){
        map_video_PTE((uint32_t)terminal[next_pointer].background_buffer);
        execute((uint8_t*)"shell");
    }
    // remap process 
    mapping_vir_to_phy(VIRTUAL_PAGE_START, PHYS_PROGRAM_START + next_pid * PHYS_PROGRAM_SIZE);
    
    if (next_pointer == curr_term_id){
        //if next terminal is activated and displayed
        //recognizes the active terminal and allows writing to the actual video memory.
        map_video_PTE(video_memory_start);

    }else{
        //if next termnial is not activated, write into backup buffer
        map_video_PTE((uint32_t)terminal[next_pointer].background_buffer);
    }

    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - next_pid * EIGHT_KB - 4;  
    // pcb_t* current_pcb = (pcb_t*)(EIGHT_MB - (current_pid+1) * EIGHT_KB);
    pcb_t* next_pcb = (pcb_t*)(EIGHT_MB - (next_pid+1) * EIGHT_KB);
    // asm volatile(
    //     "movl %%esp, %%eax;"
    //     "movl %%ebp, %%ebx;"
    //     :"=a"(current_pcb->esp_val),"=b"(current_pcb->ebp_val)
    //     :
    // );
    // asm volatile(
    //     "movl %%eax, %%esp;"
    //     "movl %%ebx, %%ebp;"
    //     :
    //     :"a"(next_pcb->esp_val),"b"(next_pcb->ebp_val)
    // );
    asm volatile(
        "movl %0, %%ebp \n\
                leave          \n\
                ret            \n"
            :  /* no output */
            :  "r" (next_pcb->sch_ebp) \
            :  "ebp");
}

/**
 * scheduler_getnext
 * 
 * Description: get next process
 *              
 * Input: none
 * Return value: array index of next process
 * Side effect: None
*/
int32_t scheduler_getnext(){
    int32_t current_pointer = myScheduler.cur_task;        // pointer of current process
    int8_t i;
    int32_t next_pointer = current_pointer;

    for (i=0;i<3;i++){
        next_pointer = (next_pointer+1)%3;
        if (myScheduler.tasks[next_pointer] != NOT_EXIST){break;}
    }
    return next_pointer;
}
