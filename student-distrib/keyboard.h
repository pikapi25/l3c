/* keyboard.h - Defines used in interactions with keyboard
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"


/* Ports that KEYBOARD sits on */
#define KEY_BUF_PORT    0x60
#define KEY_REG_PORT    0x64

/* keyboard use irq 1 on MASTER_PIC */
#define KEY_IRQ_NUM 1


/* About scan code */
#define SCAN_CODE_PRESS 59
#define RELEASE_MASK 0X80


/* Initialize keyboard */
extern void key_init(void);
/* keyboard handler */
extern void key_handler(void);

#endif /* _KEYBOARD_H */
