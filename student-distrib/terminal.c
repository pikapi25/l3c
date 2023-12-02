/* terminal.h - Defines functions and structure for terminal driver
 * vim:ts=4 noexpandtab
 */
#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "paging.h"
#include "scheduler.h"


/* terminal_init
 * Side effect: initialize the terminal
 */
void terminal_init() {
    int i;
    for (i=0; i<NUM_TERMS; i++){
        terminal[i].background_buffer = (int32_t)(VIDEO_MEM_LOC+ VIDEO_MEM_SIZE*(i+2));
        terminal[i].readkey = 0;
        terminal[i].kbd_buf_count = 0;
        memset(terminal[i].kbd_buf, 0, MAX_CHA_BUF);   //not sure
        terminal[i].cursor_x = 0;  //the location of the cursor at the beginning
        terminal[i].cursor_y = 0;
        // enable_cursor(0, 0);							
        update_cursor(terminal[i].cursor_x, terminal[i].cursor_y);	//draw the initial cursor position
    }
}
   

/* terminal_switch
 * switch to another terminal
 * Input: new_term, the id of destination terminal (from 0 to 2)
 * Output: return none, display new terminal on screen through videomap
*/
void terminal_switch(uint8_t new_term){
    if (new_term < 0 || new_term > 2){return;}
    /* update video memory paging */
    update_vidmem_paging(curr_term_id);

    /* copy from video memory to backgroud buffer of current terminal */
    memcpy(terminal[curr_term_id].background_buffer, (uint8_t*)VIDEO_MEM_LOC, VIDEO_MEM_SIZE);

    /* load background buffer of the target into video memory */
    memcpy((uint8_t*)VIDEO_MEM_LOC, terminal[new_term].background_buffer, VIDEO_MEM_SIZE);

    /* update video memory paging */
    update_vidmem_paging(new_term);
}

/* reset_kbd_buf
 * helper function to reset keyboard_buffer to 0 
 * Side effect: keyboard_buffer is initialized
 */
void reset_kbd_buf(int term_id) {
    //emptify the buffer
    terminal[term_id].kbd_buf_count = 0;
    memset(terminal[term_id].kbd_buf, 0, MAX_CHA_BUF); //not sure
}

/* get_terminal
 * return the address of the current terminal
 * Input: none
 * Return value: terminal_t* terminal
 */
terminal_t* get_terminal(){
    return &(terminal[curr_term_id]);
}

/* terminal_read
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
	int32_t i = 0;
    int8_t curr_sch_id = myScheduler.cur_task;
	if (NULL == buf || nbytes <= 0) { return 0; }

	reset_kbd_buf(curr_sch_id);
    terminal[curr_sch_id].readkey = 1;
	while (terminal[curr_sch_id].readkey);					//Wait for the flag 
	/* Read from the keyboard buffer */
	/* User can only type up to 127 (MAX_CHA_BUF - 1) characters */
	for (i = 0; i < nbytes && i < MAX_CHA_BUF && terminal[curr_sch_id].kbd_buf[i] != '\0'; i++) {
		((char*)buf)[i] = terminal[curr_sch_id].kbd_buf[i];
	}
    reset_kbd_buf(curr_sch_id);
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
        // if( c == '\0' || c == 0 ) break;
        user_terminal_putc(c, 0);
        //reset_kbd_buf();
        num_put++;
    }
    return num_put;
}

/* clear_redraw
 * clear the screen and move the cursor position to the beginning
 * Side effect: Clear the screen
 */
void clear_redraw() {
    clean();
}

int32_t illegal_open(const uint8_t* filename) {
	printf("FAIL TO OPEN TERMINAL\n");
	return -1;
}

int32_t illegal_close(int32_t fd) {
	printf("FAIL TO CLOSE TERMINAL\n");
	return -1;
}

int32_t illegal_read(int32_t fd, void* buf, int32_t nbyte){
    printf("FAIL TO READ\n");
    return -1;
}

int32_t illegal_write(int32_t fd, const void* buf, int32_t nbyte){
    printf("FAIL TO WRITE\n");
    return -1;
}


