#ifndef _COMMAND_HISTORY_H
#define _COMMAND_HISTORY_H

#include "types.h"

#define MAX_HISTORY 30

uint8_t history_buf[MAX_HISTORY][128];
int32_t curr_history_index;

extern void press_up();
extern void press_down();
extern void update_index();

extern void clear_buff(int32_t i);


#endif
