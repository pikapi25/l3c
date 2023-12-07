#include "mouse.h"
#include "lib.h"
#include "i8259.h"
#include "vga.h"
#include "gui.h"

/* mouse_wait: wait until mouse can read or write
 * Input: wait_type, tell the function whether we are waiting for read or write
 * Output: None
 * Side Effect: May cause dead loop!
*/
void mouse_wait(uint8_t wait_type){
    int32_t counter = 10000;
    if(wait_type == WAIT_MOUSE_READ){
        while(counter--){
            if((inb(MOUSE_CHECK_PORT) & WAIT_MOUSE_READ) == MOUSE_CAN_READ){
                break;
            }
        }
    }else if(wait_type == WAIT_MOUSE_WRITE){
        while(counter--){
            if((inb(MOUSE_CHECK_PORT) & WAIT_MOUSE_WRITE) == MOUSE_CAN_WRITE){
                break;
            }
        }
    }else{
        return;
    }
}

/* mouse_write: helper function to send a command to mouse port
 * Input: command
 * output: None
 * Side Effect: First wait, then tell the mouse we are sending a command;
 * Then wait, and tell what command we are sending;
 * In the end, wait for ack. ???
*/
void mouse_write(uint8_t command){
    mouse_write_port(MOUSE_GOING_TO_WRITE, MOUSE_CHECK_PORT);
    mouse_write_port(command, MOUSE_DATA_PORT);
    mouse_read();
}

/* mouse_write_port: helper function to write a data to port.
 * Input: data, port
 * Output: None
 * Side Effect: Wait until mouse can write, then outb(data, port)
*/
void mouse_write_port(uint8_t data, uint8_t port){
    mouse_wait(WAIT_MOUSE_WRITE);
    outb(data, port);
}

/* mouse_read: helper function to read from mouse
 * Input: None
 * Output: The data read from mouse data port
 * Side Effect: Need to wait until we can read.
*/
uint8_t mouse_read(){
    mouse_wait(WAIT_MOUSE_READ);
    return inb(MOUSE_DATA_PORT);
}

/* mouse_init: enable and initialize the mouse
 * Input: None
 * Output: None
 * Side Effect: None
*/
void mouse_init(){
    //uint8_t i;
    mouse_write(MOUSE_AUX);
    uint8_t status;
    // Enable the interrupts
    mouse_write_port(MOUSE_ENABLE_INT, MOUSE_CHECK_PORT);
    status = mouse_read();
    status = (status | 2)& 0xDF;
    mouse_write_port(MOUSE_DATA_PORT, MOUSE_CHECK_PORT);
    mouse_write_port(status, MOUSE_DATA_PORT);

    // Tell the mouse to use default settings
    mouse_write(MOUSE_DEFAULT_SET);

    // Enable the mouse
    mouse_write(MOUSE_ENABLE);

    // mouse_write_port(MOUSE_GOING_TO_WRITE, MOUSE_CHECK_PORT);
    // mouse_write_port(0xF3, MOUSE_DATA_PORT);
    // mouse_write_port(200, MOUSE_DATA_PORT);
    // for(i = 0; i < NUM_TERMS; i++){
    //     my_mouse[i].mouse_left_btn = BTN_NOT_PRESSED;
    //     my_mouse[i].mouse_middle_btn = BTN_NOT_PRESSED;
    //     my_mouse[i].mouse_right_btn = BTN_NOT_PRESSED;
    //     my_mouse[i].mouse_x = NUM_COLS / 2;
    //     my_mouse[i].mouse_y = NUM_ROWS / 2;
    //     my_mouse[i].mouse_prev_x = NUM_COLS / 2;
    //     my_mouse[i].mouse_prev_y = NUM_ROWS / 2;
    //     my_mouse[i].prev_c = SPACE_CHAR;
    // }
    my_mouse.mouse_left_btn = BTN_NOT_PRESSED;
    my_mouse.mouse_middle_btn = BTN_NOT_PRESSED;
    my_mouse.mouse_right_btn = BTN_NOT_PRESSED;
    my_mouse.mouse_x = VGA_DIMX / 2;
    my_mouse.mouse_y = VGA_DIMY / 2;
    my_mouse.mouse_prev_x = VGA_DIMX / 2;
    my_mouse.mouse_prev_y = VGA_DIMY / 2;
    my_mouse.prev_c = SPACE_CHAR;

    //set_mouse_cursor(DEFAULT_MOUSE_CHAR);
    enable_irq(MOUSE_IRQ);
}

/* mouse_handler: handle mouse interrupt, update mouse values
 * Input: None
 * Output: None
 * Side Effect: data in my_mouse will be updated.
*/
void mouse_handler(){

    send_eoi(MOUSE_IRQ);
    uint8_t pkt1;
    uint8_t pkt2;
    uint8_t pkt3;
    int32_t x_mov;
    int32_t y_mov;
    //uint8_t c = DEFAULT_MOUSE_CHAR;
    if (0 == (inb(MOUSE_CHECK_PORT) & 0x1)){
        return;
    }
    if (0 == (inb(MOUSE_CHECK_PORT) & 0x20)){
        return;
    }
    pkt1 = inb(MOUSE_DATA_PORT);
    //pkt1 = mouse_read();
    if(MOUSE_ACK == pkt1){
        return;
    }else{
        if(!(pkt1 & MOUSE_ALWAYS_ONE)) return;
        if ((pkt1 & MOUSE_X_OVERFLOW) || (pkt1 & MOUSE_Y_OVERFLOW)){
            //printf("mouse overflow!\n");
            return;
        } 
        pkt2 = mouse_read();
        pkt3 = mouse_read();
        if (pkt1 & MOUSE_LEFT_BTN){
            mouse_left_click();
            //c = '@';
            //printf("left button!\n");
        }else if (pkt1 & MOUSE_RIGHT_BTN){
            //c = '#';
            //printf("right button!\n");
        }else if (pkt1 & MOUSE_MID_BTN){
            //c = '*';
            //printf("middle button!\n");
        }
        if (pkt1 & MOUSE_X_NEG){
            x_mov = (int32_t)(MOUSE_NEG_MASK | pkt2) / 10;
        }else{
            x_mov = (int32_t)pkt2 / 10;
        }
        if (pkt1 & MOUSE_Y_NEG){
            y_mov = (int32_t)(MOUSE_NEG_MASK | pkt3) / 10;
        }else{
            y_mov = (int32_t)pkt3 / 10;
        }
        if (my_mouse.mouse_x + x_mov < 0){
            my_mouse.mouse_x = 0;
        } else if (my_mouse.mouse_x + x_mov > VGA_DIMX - 17){
            my_mouse.mouse_x = VGA_DIMX - 17;
        }else{
            my_mouse.mouse_x = my_mouse.mouse_x + x_mov;
        }

        if (my_mouse.mouse_y - y_mov < 0){
            my_mouse.mouse_y = 0;
        } else if (my_mouse.mouse_y - y_mov > VGA_DIMY - 17){
            my_mouse.mouse_y = VGA_DIMY - 17;
        }else{
            my_mouse.mouse_y = my_mouse.mouse_y - y_mov;
        }

    }
    //set_mouse_cursor(c);
    
}

uint8_t check_in_term(int term_id){
    if(my_mouse.mouse_x >= term_window[term_id].x_coord && my_mouse.mouse_x < term_window[term_id].x_coord + WINDOW_WIDTH){
        if(my_mouse.mouse_y >= term_window[term_id].y_coord && my_mouse.mouse_y < term_window[term_id].y_coord + WINDOW_HEIGHT + WINDOW_TITLE_HEIGHT){
            return 1;
        }
    }
    return 0;
}

void mouse_left_click(){
    int i;
    for (i = NUM_TERMS - 1; i >=0; i--){
        if(check_in_term(term_orders[i])){
            terminal_switch(term_orders[i]);
            break;
        }
    }
}
