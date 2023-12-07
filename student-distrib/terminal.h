/* terminal.h - Defines functions and structure for terminal driver
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "lib.h"

#define NUM_TERMS 3
#define MAX_CHA_BUF 128
#define VIDEO_MEM_LOC   VIDEO
#define VIDEO_MEM_SIZE 0x01000


/* Terminal struct, used to keep all informations related to a terminal */
typedef struct terminal_t {
	volatile uint8_t readkey;   /* enable/disable reading from kbd buffer   */
	uint8_t  cursor_x;          /* column number of current cursor position */
	uint8_t  cursor_y;          /* row number of current cursor position    */
	uint8_t  kbd_buf[MAX_CHA_BUF];     /* keyboard buffer                  */
	uint16_t  kbd_buf_count;     /* number of characters in keyboard buffer  */
	uint8_t* background_buffer; /* address of the background buffer */
} terminal_t;

/*Initialize the terminal*/
extern void terminal_init();
/*Switch terminal*/
extern void terminal_switch(uint8_t new_term);
/*reset the keyboard buffer*/
void reset_kbd_buf(int term_id);
/*return current terminal_t* */
extern terminal_t* get_terminal();
/* Read from the terminal */
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
/* Write to the terminal */
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
/* Clear the screen and redraw the cursor at current position */
extern void clear_redraw();

int32_t illegal_open(const uint8_t* filename);

int32_t illegal_close(int32_t fd);

int32_t illegal_read(int32_t fd, void* buf, int32_t nbyte);

int32_t illegal_write(int32_t fd, const void* buf, int32_t nbyte);

terminal_t terminal[NUM_TERMS]; //terminal
volatile uint8_t curr_term_id; // current(visible) terminal id
#endif /* _TERMINAL_H */
