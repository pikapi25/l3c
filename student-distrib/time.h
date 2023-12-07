#ifndef _TIME_H
#define _TIME_H

#include "types.h"

int second, minute, hour, day, month, year;
int last_second, last_minute, last_hour, last_day, last_month, last_year;
char time_buf[18];
extern void get_time();

#endif
