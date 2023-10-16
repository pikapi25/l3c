#define ASM     1
#include "interrupt_linkage.h"

#define INTR_LINK(name, func)   \
    .globl name                ;\
    name:                      ;\
        pushal                 ;\
        pushfl                 ;\
        call func              ;\
        popfl                  ;\
        popal                  ;\
        iret 

#create the assemply linkage for keyboard interrupt
INTR_LINK(keyboard_handler_linkage, keyboard_handler);   

#create the assemply linkage for RTC interrupt
INTR_LINK(rtc_handler_linkage, rtc_handler);         
