#ifndef _MOUSE_H
#define _MOUSE_H

#include "types.h"
#include "terminal.h"

#define MOUSE_IRQ 12
#define MOUSE_CHECK_PORT 0x64
#define MOUSE_DATA_PORT 0x60

// Write must wait Port 0x64 bit 0 to be cleared
// Read must wait Port 0x64 bit 1 to be set
#define WAIT_MOUSE_READ 1
#define WAIT_MOUSE_WRITE 2
#define MOUSE_CAN_READ 1
#define MOUSE_CAN_WRITE 0
// we need to tell it we are sending a command before actually sending
#define MOUSE_GOING_TO_WRITE 0xD4 
#define MOUSE_AUX 0xA8
#define MOUSE_ENABLE_INT 0x20
#define MOUSE_DEFAULT_SET 0xF6
#define MOUSE_ENABLE 0xF4



#define MOUSE_STATUS_MASK 0xDF

#define MOUSE_LEFT_BTN 0x01
#define MOUSE_RIGHT_BTN 0x02
#define MOUSE_MID_BTN 0x04
#define MOUSE_Y_OVERFLOW 0x80
#define MOUSE_X_OVERFLOW 0x40
#define MOUSE_Y_NEG 0x20
#define MOUSE_X_NEG 0x10
#define MOUSE_ALWAYS_ONE 0x08

#define DEFAULT_MOUSE_CHAR 0x7C
#define SPACE_CHAR 0x20
#define BTN_NOT_PRESSED 0
#define BTN_PRESSED 1

#define MOUSE_ACK 0xFA
#define MOUSE_NEG_MASK 0xFFFFFF00


typedef struct{
    uint8_t mouse_left_btn;
    uint8_t mouse_right_btn;
    uint8_t mouse_middle_btn;
    int32_t mouse_x;
    int32_t mouse_y;
    int32_t mouse_drag_x;
    int32_t mouse_drag_y;
    int drag_term;
    uint8_t prev_c;
}mouse_t;

mouse_t my_mouse;


void mouse_init();
void mouse_handler();

/* ----- Helper Functions ----- */
void mouse_wait(uint8_t wait_type);
void mouse_write(uint8_t command);
void mouse_write_port(uint8_t command, uint8_t port);
uint8_t mouse_read();
uint8_t check_in_term(int term_id);
void mouse_left_click();
#endif /* _MOUSE_H */
