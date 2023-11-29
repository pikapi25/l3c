#include "scheduler.h" 
#include "lib.h"


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
void PIT_init()
{
    //set the value of PIT mode/command register
    outb(PIT_REG, PIT_IO_PORT);

    //set frequency to channel 0 data port
    outb(PIT_FREQ & LOWER_MASK, CHANNEL_0);
    outb(PIT_FREQ & HIGHER_MASK, CHANNEL_0);

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