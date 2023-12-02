#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include "lib.h"
#include "paging.h"
#include "terminal.h"
#include "x86_desc.h"
#define PIT_IO_PORT         0x43

//0x36 = 00110110 = channel 0(00)|access mode 3(11)|operating mode 3(011)|16-bit(0)
#define PIT_REG             0x36
#define CHANNEL_0           0x40
#define VIRTUAL_VIDEO       0xb8000
#define PIT_FREQ            11931       //The oscillator used by the PIT chip runs at (roughly) 1.193182 MHz. 
#define LOWER_MASK          0x00FF      //low byte         
#define HIGHER_MASK         0xFF00      //high byte

#define PIT_IRQ             0           //PIT has top priority
#define NOT_EXIST           0           //denote when the process is not created or has been completed and removed
#define NUM_SCHES      3  // number of schedules
typedef struct{
    int32_t tasks[NUM_SCHES];
    int8_t num_tasks;                   // current tasks number
    int8_t cur_task;                    // current active task id 
}Scheduler;

Scheduler myScheduler;

extern void pit_handler( void );
extern void PIT_init( void );
extern void scheduler_initialize();

//helper function
void scheduler();
int8_t scheduler_getnext();
#endif
