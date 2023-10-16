#ifndef INTERRUPT_LINKAGE_H
#define INTERRUPT_LINKAGE_H

// link the keyboard interrupt handler
extern void keyboard_handler_linkage();

// link the RTC interrupt handler
extern void rtc_handler_linkage();

#endif
