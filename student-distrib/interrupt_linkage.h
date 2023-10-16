#ifndef _INTERRUPT_LINKAGE_H
#define _INTERRUPT_LINKAGE_H
#ifndef ASM

// #include "keyboard.h"
// #include "rtc.h"

// link the keyboard interrupt handler
extern void keyboard_handler_linkage();

// link the RTC interrupt handler
extern void rtc_handler_linkage();

#endif
#endif
