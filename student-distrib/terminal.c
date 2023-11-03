/* terminal.h - Defines functions and structure for terminal driver
 * vim:ts=4 noexpandtab
 */
#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "paging.h"

terminal_t terminal; 

/* terminal_init
 * Side effect: initialize the terminal
 */
void terminal_init() {
    terminal.readkey = 0;
    terminal.kbd_buf_count = 0;
    memset(terminal.kbd_buf, 0, MAX_CHA_BUF);   //not sure
    terminal.cursor_x = 0;  //the location of the cursor at the beginning
    terminal.cursor_y = 0;
    // enable_cursor(0, 0);							
    update_cursor(terminal.cursor_x, terminal.cursor_y);	//draw the initial cursor position
}

/* reset_kbd_buf
 * helper function to reset keyboard_buffer to 0 
 * Side effect: keyboard_buffer is initialized
 */
void reset_kbd_buf() {
    //emptify the buffer
    terminal.kbd_buf_count = 0;
    memset(terminal.kbd_buf, 0, MAX_CHA_BUF); //not sure
}

/* get_terminal
 * return the address of the current terminal
 * Input: none
 * Return value: terminal_t* terminal
 */
terminal_t* get_terminal(){
    return &(terminal);
}

/* terminal_read
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
	int32_t i = 0;
	if (NULL == buf || nbytes <= 0) { return 0; }

	reset_kbd_buf();
    terminal.readkey = 1;
	while (terminal.readkey);					//Wait for the flag 
	/* Read from the keyboard buffer */
	/* User can only type up to 127 (MAX_CHA_BUF - 1) characters */
	for (i = 0; i < nbytes && i < MAX_CHA_BUF && terminal.kbd_buf[i] != '\0'; i++) {
		((char*)buf)[i] = terminal.kbd_buf[i];
	}
    reset_kbd_buf();
	return i;
}

/* terminal_write
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
	int i = 0;

	if (NULL == buf || nbytes == 0) { return -1; }

    const uint8_t* write_buf = buf;
    uint8_t c;
    uint32_t num_put = 0;
    for( i = 0; i < nbytes; i++ )
    {
        c = write_buf[i];
        /* If character is not valid, then break.   */
        if( c == '\0' || c == 0 ) break;
        userkey_putc(c);
        reset_kbd_buf();
        num_put++;
    }
    return num_put;
}

/* clear_redraw
 * clear the screen and move the cursor position to the beginning
 * Side effect: Clear the screen
 */
void clear_redraw() {
    int i;
    //clear video memory by setting all to ' ' and ATTRIB
    for( i = 0; i < NUM_ROWS*NUM_COLS; i++ ){
        *(uint8_t *)(VIDEO_MEM_LOC + (i << 1)) = ' ';
        *(uint8_t *)(VIDEO_MEM_LOC + (i << 1) + 1) = ATTRIB;
    }

    terminal.cursor_x = 0;
    terminal.cursor_y = 0;

    //redraw the cursor
    update_cursor(terminal.cursor_x, terminal.cursor_y);

    return;
}

