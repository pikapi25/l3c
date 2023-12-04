#include "mouse.h"
#include "lib.h"


int16_t mouse_x;
int16_t mouse_y;
int16_t prev_mouse_x;
int16_t prev_mouse_y;
int32_t prev_draw_x;
int32_t prev_draw_y;
int32_t need_play;

uint8_t mouse_cycle = MOUSE_PACKET_1;
uint8_t mouse_packet[3];
uint8_t mouse_ack = MOUSE_HANDLING;

/* mouse_wait: wait until mouse can read or write
 * Input: wait_type, tell the function whether we are waiting for read or write
 * Output: None
 * Side Effect: May cause dead loop!
*/
void mouse_wait(uint8_t wait_type){
    if(wait_type == WAIT_MOUSE_READ){
        while(1){
            if((inb(MOUSE_CHECK_PORT) & WAIT_MOUSE_READ) == MOUSE_CAN_READ){
                break;
            }
        }
    }else if(wait_type == WAIT_MOUSE_WRITE){
        while(1){
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
    /* not sure if we need to wait for ack. */
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
    uint8_t status;
    // Enable the interrupts
    mouse_write_port(MOUSE_ENABLE_INT, MOUSE_CHECK_PORT);
    status = mouse_read();
    status = status | 2;
    mouse_write_port(MOUSE_DATA_PORT, MOUSE_CHECK_PORT);
    mouse_write_port(status, MOUSE_DATA_PORT);

    // Tell the mouse to use default settings
    mouse_write(MOUSE_DEFAULT_SET);

    // Enable the mouse
    mouse_write(MOUSE_ENABLE);
}

/* mouse_handler: handle mouse interrupt, update mouse values
 * Input: None
 * Output: None
 * Side Effect: data in my_mouse will be updated.
*/
void mouse_handler(){
    uint8_t input;
    switch (mouse_cycle)
    {
        case MOUSE_PACKET_1:
            mouse_packet[MOUSE_PACKET_1] = inb(MOUSE_DATA_PORT);
            mouse_cycle++;
            break;
        case MOUSE_PACKET_2:
            mouse_packet[MOUSE_PACKET_2] = inb(MOUSE_DATA_PORT);
            mouse_cycle++;
            break;
        case MOUSE_PACKET_3:
            mouse_packet[MOUSE_PACKET_3] = inb(MOUSE_DATA_PORT);
            mouse_cycle = MOUSE_PACKET_1;
            mouse_ack = MOUSE_HANDLE_DONE;
            break;
    }
    if(mouse_ack == MOUSE_HANDLE_DONE){
        if(mouse_packet[MOUSE_PACKET_1] & MOUSE_LEFT_BTN){
            printf("left button!\n");
        }
        if(mouse_packet[MOUSE_PACKET_1] & MOUSE_RIGHT_BTN){
            printf("rightbutton!\n");
        }
        if(mouse_packet[MOUSE_PACKET_1] & MOUSE_MID_BTN){
            printf("middle button!\n");
        }
    }
    mouse_ack = MOUSE_HANDLING;
}