#include "time.h"
#include "rtc.h"
#include "lib.h"
#include "terminal.h"
#include "gui.h"
// Reference: https://wiki.osdev.org/CMOS#Getting_Current_Date_and_Time_from_RTC


int get_update_in_progress_flag() {
    int cmos_data;
    outb(0x0A, 0x70);
    cmos_data = inb(0x71);
    return (cmos_data & 0x80);
}

int get_RTC_register(int reg) {
    int cmos_data;
    outb(reg, 0x70);
    cmos_data = inb(0x71);
    return cmos_data;
}

void get_time() {
    while (get_update_in_progress_flag());
    second = get_RTC_register(0x00);
    minute = get_RTC_register(0x02);
    hour = get_RTC_register(0x04);
    day = get_RTC_register(0x07);
    month = get_RTC_register(0x08);
    year = get_RTC_register(0x09);

    do {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;
 
        while (get_update_in_progress_flag()); // Make sure an update isn't in progress
        second = get_RTC_register(0x00);
        minute = get_RTC_register(0x02);
        hour = get_RTC_register(0x04);
        day = get_RTC_register(0x07);
        month = get_RTC_register(0x08);
        year = get_RTC_register(0x09);
    } while ((last_second != second) || (last_minute != minute) || (last_hour != hour) ||
             (last_day != day) || (last_month != month) || (last_year != year));

    int registerB = get_RTC_register(0x0B);
 
      // Convert BCD to binary values if necessary
 
    if (!(registerB & 0x04)) {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
    }

    // Convert 12 hour clock to 24 hour clock if necessary

    if (!(registerB & 0x02) && (hour & 0x80)) {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    int_to_char(year, time_buf, 3, 0);
    time_buf[4] = '/';
    int_to_char(month, time_buf, 6, 4);
    time_buf[7] = '/';
    int_to_char(day, time_buf, 9, 7);
    time_buf[10] = ' ';
    int_to_char((hour+18)%24, time_buf, 12, 10);
    time_buf[13] = ':';
    int_to_char(minute, time_buf, 15, 13);
    time_buf[16] = ':';
    int_to_char(second, time_buf, 18, 16);
}



 


