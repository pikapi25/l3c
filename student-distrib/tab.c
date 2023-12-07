#include "tab.h"
#include "lib.h"
#include "scheduler.h"
#include "paging.h"
#include "command_history.h"
#include "terminal.h"

static uint8_t commandStrings[9][STRING_SIZE] = {
    "testprint",
    "counter",
    "pingpong",
    "syserr",
    "cat",
    "grep",
    "shell",
    "fish",
    "ls"
};

void press_tab(){
    int j = 0;

    uint8_t string[STRING_SIZE];

    for (j = 0; j < 10; j++){
        if (j == 9) {return;}
        if ((terminal[curr_term_id].kbd_buf[0] == commandStrings[j][0]) && (terminal[curr_term_id].kbd_buf[1] == commandStrings[j][1])){
            memset(string, 0, STRING_SIZE);
            memcpy(string, commandStrings[j], STRING_SIZE);
            break;
        }
}


    clear_buff(0);
    memset((uint8_t*) terminal[curr_term_id].kbd_buf,0,128);

    j=0;
    while (j < STRING_SIZE && string[j] != '\0'){
        update_vidmem_paging(curr_term_id);
        user_terminal_putc(string[j],1);
        update_vidmem_paging(myScheduler.cur_task);
        terminal[curr_term_id].kbd_buf[j] = string[j];
        j++;
    }
    terminal[curr_term_id].kbd_buf_count = j;
}
