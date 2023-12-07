#ifndef _INTERRUPT_LINKAGE_H
#define _INTERRUPT_LINKAGE_H
#ifndef ASM

// #include "keyboard.h"
// #include "rtc.h"

//the assemply linkage for keyboard interrupt
extern void keyboard_handler_linkage();

//the assemply linkage for RTC interrupt
extern void rtc_handler_linkage();

//the assemply linkage for PIT interrupt
extern void pit_handler_linkage();

extern void sb16_handler_linkage();

#endif
#endif
