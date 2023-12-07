#include "command_history.h"
#include "terminal.h"
#include "lib.h"
#include "paging.h"
#include "scheduler.h"


static int32_t print_history_index = 0;             //denote command index to be printed
static int32_t num_count = 0;
static int32_t overload = 0;
// Reference: https://github.com/unw9527/ECE391-LLLOS/blob/master/student-distrib

void update_buf(int32_t i){
    clear_buff(0);

    memset((uint8_t*) terminal[curr_term_id].kbd_buf,0,128);

    while (i < 128 && history_buf[print_history_index][i] != 0x0A){
        update_vidmem_paging(curr_term_id);
        user_terminal_putc(history_buf[print_history_index][i],1);
        update_vidmem_paging(myScheduler.cur_task);
        terminal[curr_term_id].kbd_buf[i] = history_buf[print_history_index][i];
        i++;
    }
    terminal[curr_term_id].kbd_buf_count = i;
}

void press_up(){
    cli();
    int32_t i = 0;
    print_history_index--;

    if (print_history_index < 0){
        if (overload){
            print_history_index = MAX_HISTORY - 1;
        }else{
            print_history_index = 0;
            sti();
            return;
        }
    }
    if (num_count == MAX_HISTORY){
        print_history_index ++;
        sti();
        return;
    }
    update_buf(i);
    num_count++;
    sti();
}


void press_down(){
    cli();
    int32_t i = 0;

    if (num_count == 0){
        sti();
        return;
    }else if (num_count == 1){
        print_history_index++;
        num_count--;
        clear_buff(0);
        memset((uint8_t*) terminal[curr_term_id].kbd_buf,0,128);
        sti();
        return;
    }else{
        print_history_index++;
        num_count--;
    }

    if (print_history_index == MAX_HISTORY){
            print_history_index = 0;
    }

    update_buf(i);
    sti();
}

void update_index(){
    cli();
    if (curr_history_index == MAX_HISTORY){
        curr_history_index = 0;
        overload = 1;
    }
    else{
        if (terminal[curr_term_id].kbd_buf_count == 0){
            print_history_index = curr_history_index;
            sti();
            return;
        }
        curr_history_index ++;
        print_history_index = curr_history_index;
        num_count = 0;
    }
    sti();
}

void clear_buff(int32_t i ){
    while (terminal[curr_term_id].kbd_buf_count >i){
        update_vidmem_paging(curr_term_id);
        terminal[curr_term_id].kbd_buf_count--;
        handle_backspace(1);
        update_cursor(terminal[curr_term_id].cursor_x, terminal[curr_term_id].cursor_y);
        update_vidmem_paging(myScheduler.cur_task);

    }
}
