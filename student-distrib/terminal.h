/* terminal.h - Defines functions and structure for terminal driver
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"

#define MAX_CHA_BUF 128
#define VIDEO_MEM_LOC   0xB8000
#define NUM_COLS        80
#define NUM_ROWS        25
#define ATTRIB          0x7

/* Terminal struct, used to keep all informations related to a terminal */
typedef struct terminal_t {
	volatile uint8_t readkey;   /* enable/disable reading from kbd buffer   */
	uint8_t  cursor_x;          /* column number of current cursor position */
	uint8_t  cursor_y;          /* row number of current cursor position    */
	uint8_t  kbd_buf[MAX_CHA_BUF];     /* keyboard buffer                  */
	uint16_t  kbd_buf_count;     /* number of characters in keyboard buffer  */
} terminal_t;

/*Initialize the terminal*/
extern void terminal_init();
/*reset the keyboard buffer*/
void reset_kbd_buf();
/*return current terminal_t* */
extern terminal_t* get_terminal();
/* Read from the terminal */
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
/* Write to the terminal */
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
/* Clear the screen and redraw the cursor at current position */
extern void clear_redraw();

extern terminal_t terminal; //terminal

#endif /* _TERMINAL_H */