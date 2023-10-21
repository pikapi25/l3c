/* rtc.c - Functions to interact with the 8259 interrupt controller */

#include "i8259.h"
#include "lib.h"
#include "rtc.h"

volatile uint8_t rtc_int_flag = 0; // indicate if an interrupt occurred. 0 for not occurred.
volatile uint32_t rtc_ticks = 0; // the number of ticks. every **actual** interrupt increment one tick.
uint32_t rtc_virtual_rate = 2; /* the virtual interrupt rate */

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
    /* Virtualize interrupt rate:
     * say, if the virtual rate is r, it means 1/r seconds per interrupt
     * however, the actual rate is 1024, which means it is 1/1024 seconds per interrupt
     * therefore, 1/r / (1/1024) = 1024 / r actual interrupts is equal to one virtual interrupt
    */
    cli();
    rtc_ticks ++;
    if (rtc_ticks % (RTC_DEFAULT_RATE / rtc_virtual_rate) == 0){
        rtc_int_flag = 1;
    }
    outb(RTC_REG_C, RTC_REG_PORT); // select register C
    inb(RTC_CMOS_RW_PORT); // just throw away contents
    //test_interrupts(); // for testing
    sti();
    send_eoi(RTC_IRQ_NUM); // send end-of-interrupt to pic
     
}

/* rtc_open
 * INPUT: filename
 * OUTPUT: 0
 * EFFECT: set the interrupt rate to 2
*/
int32_t rtc_open(const uint8_t* filename){
    rtc_virtual_rate = 2;
    return 0;
}

/* rtc_close
 * INPUT: file descriptor
 * OUTPUT: 0
 * EFFECT: none
*/
int32_t rtc_close(int32_t fd){
    return 0;
}

/* rtc_read
 * INPUT: file descriptor fd, buffer buf, and number of byte nbytes
 * OUTPUT: 0
 * EFFECT: wait until RTC interrupt occurred, and return 0
*/
int32_t rtc_read(int32_t fd, const void* buf, int32_t nbytes){
    /* wait until interrupt occured */
    while (!rtc_int_flag);
    rtc_int_flag = 0;
    return 0;
}

/* rtc_write
 * INPUT: file descriptor fd, buffer buf, and number of byte nbytes
 * OUTPUT: 0 for success and -1 for failuer
 * EFFECT: set the required interrupt rate read from buf
*/
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    /* read the required frequency from buf */
    uint32_t rtc_set_freq = 0;
    if (buf != NULL){
        rtc_set_freq = *(uint32_t*)buf;
    }else{
        return -1;
    }

    /* check if the required frequency is a power of 2, and in range (0, 1024]
     * A power of 2 has exactly one bit set in its binary representation.
     * If we subtract 1 from a power of 2, all its bits will be set to 1 except for the one we set to 0.
     * For example: 8 (1000) - 1 = 7 (0111)
     * If we AND the original number with (num - 1), it should result in 0. */

    if (rtc_set_freq > 0 && rtc_set_freq <= 1024 && ((rtc_set_freq & (rtc_set_freq - 1)) == 0 )){
        rtc_virtual_rate = rtc_set_freq;
    }else{
        return -1;
    }
    return 0;
}