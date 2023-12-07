#ifndef _TIME_H
#define _TIME_H

#include "types.h"

int32_t second, minute, hour, day, month, year;
int32_t last_second, last_minute, last_hour, last_day, last_month, last_year;

extern void get_time();

#endif
