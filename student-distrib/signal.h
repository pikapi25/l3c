#ifndef SIGNAL_H
#define SIGNAL_H

#include "types.h"

#define DIV_ZERO        0
#define SEGFAULT        1
#define INTERRUPT       2
#define ALARM           3
#define USER1           4
#define SIG_COUNT       5


typedef struct hwcontext {
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t eax;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t error_code;
    uint32_t return_address;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} hwcontext_t;


void DO_SIG_RETURN(void);
void DO_SIG_RETURN_END(void);
extern void kill_task(void);
extern void ignore(void);
extern void send_signal(int sig_num);
extern void handle_signal();


#endif
