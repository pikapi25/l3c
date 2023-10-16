#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"
#include "lib.h"
#include "interrupt_linkage.h"

//vectors for system call
#define SYSTEM_CALL_VEC         0x80

//vectors for interrupts
#define KEYBOARD_VEC             0x21
#define RTC_VEC                  0x28


/* Functions defined  */
void exception_handler(uint32_t vec_nr);
void system_call_handler();

#endif

