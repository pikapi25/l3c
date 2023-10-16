/* rtc.c - Functions to interact with the 8259 interrupt controller */

#include "i8259.h"
#include "lib.h"
#include "rtc.h"

volatile uint8_t rtc_int_flag = 0; // indicate if an interrupt occurred. 0 for not occurred.
// volatile uint32_t rtc_ticks = 0; // the number of ticks. every interrupt increment one tick.


/* rtc_init
 * INPUT: none
 * OUTPUT: none
 * EFFECT: initialize the rtc, and enable irq 8
*/
void rtc_init(void){

    /* Turning on IRQ 8*/
    cli();
    char prev;
    outb(RTC_REG_B|RTC_DIS_NMI, RTC_REG_PORT); // select register B and disable NMI
    prev = inb(RTC_CMOS_RW_PORT); // read the current value of register B
    outb(RTC_REG_B|RTC_DIS_NMI, RTC_REG_PORT); // set the index again (a read will reset the index to register D)
    outb(prev|0x40, RTC_CMOS_RW_PORT); // write the previous value ORed with 0x40. This turns on bit 6 of register B

    /* Set the interrupt rate */
    outb(RTC_REG_A|RTC_DIS_NMI, RTC_REG_PORT); // set index to register A, disable NMI
    prev = inb(RTC_CMOS_RW_PORT); // get initial value of register A
    outb(RTC_REG_A|RTC_DIS_NMI, RTC_REG_PORT); // reset index to A
    outb((prev & 0xF0) | RTC_DEFAULT_RATE, RTC_CMOS_RW_PORT); // write only our rate to A. Note, rate is the bottom 4 bits.

    /* Initialize the variables */
    rtc_int_flag = 0; // no interrupt occurred
    // rtc_ticks = 0; // ticks is set to 0. 

    /* enable irq line */
    sti();
    enable_irq(RTC_IRQ_NUM); // the IRQ number of RTC is 8. we need to turn on it.
}

/* rtc_handler
 * INPUT: none
 * OUTPUT: none
 * EFFECT: handle the interrupt handler, and disable irq 8
*/
void rtc_handler(void){
    char temp;
    cli();
    rtc_int_flag = 1;
    // rtc_ticks ++;
    outb(RTC_REG_C, RTC_REG_PORT); // select register C
    temp = inb(RTC_CMOS_RW_PORT); // just throw away contents 
    test_interrupts(); // for testing
    sti();
    send_eoi(RTC_IRQ_NUM); // send end-of-interrupt to pic

}
