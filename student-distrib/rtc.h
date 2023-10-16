/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"

/* IO ports used for the RTC and CMOS 
 * Port 0x70 is used to specify an index or "register number", and to disable NMI
 * Port 0x71 is used to read or write from/to that byte of CMOS configuration space
*/
#define RTC_REG_PORT            0x70
#define RTC_CMOS_RW_PORT        0x71

/* RTC Status Registers
 * Only three bytes of CMOS RAM are used to control the RTC periodic interrupt function. 
 * They are called RTC Status Register A, B, and C. 
 * They are at offset 0xA, 0xB, and 0xC in the CMOS RAM. */
#define RTC_REG_A       0xA
#define RTC_REG_B       0xB
#define RTC_REG_C       0xC

/* Mask to disable NMI */
#define RTC_DIS_NMI     0x80

/* The IRQ NUM of RTC */
#define RTC_IRQ_NUM     8

/* RTC Frequency */
#define RTC_BASE_FREQ        32768
#define RTC_DEFAULT_RATE     6

/* Externally-visible functions */

/* Initialize RTC */
void rtc_init(void);
/* handle RTC interrupts */
void rtc_handler(void);

#endif /* _RTC_H */
