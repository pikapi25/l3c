
#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"


//keyboard buffer register is connected to port 0x60
#define KEY_BUF_PORT    0x60

//PS\2 keyboard use irq 1 of the master PIC
#define KEY_IRQ_NUM 1

//the number of valid scan code in checkpoint1 
#define SCAN_CODE_PRESS 59
//13 keys of keypad are used in checkpoint1
#define KEYPAD_NUM 13
// //keypad mask used to get ascii code
// #define KEYPAD_MASK 0x47

//scan code of modifier keys
#define LEFT_CTRL_PRESSED    0x1D
#define LEFT_SHIFT_PRESSED   0x2A
#define RIGHT_SHIFT_PRESSED  0x36
#define CAPS_LOCK_PRESSED    0x3A
#define LEFT_ALT_PRESSED     0x38
#define UP_PRESSED           0x48
#define DOWN_PRESSED         0x50
#define TAB_PRESSED          0xF

// #define NUM_LOCK_PRESSED     0x45
#define RELEASED_OFFSET      0x80
#define LEFT_CTRL_RELEASED   LEFT_CTRL_PRESSED + RELEASED_OFFSET 
#define LEFT_SHIFT_RELEASED  LEFT_SHIFT_PRESSED + RELEASED_OFFSET
#define RIGHT_SHIFT_RELEASED RIGHT_SHIFT_PRESSED + RELEASED_OFFSET
#define LEFT_ALT_RELEASED LEFT_ALT_PRESSED + RELEASED_OFFSET
#define F1_KEY		0x3B
#define F2_KEY		0x3C
#define F3_KEY		0x3D
#define TERM_ONE    0
#define TERM_TWO    1
#define TERM_THREE    2


//Initialize keyboard
extern void keyboard_init(void);

//keyboard handler function
extern void keyboard_handler(void);

#endif /* _KEYBOARD_H */
