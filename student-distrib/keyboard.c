/* keyboard.c - Functions to interact with keyboard
 */

#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

/* Scan code for special keys */
#define LEFT_CTRL_PRESSED    0x1D
#define LEFT_CTRL_RELEASED   0x9D
#define LEFT_ALT_PRESSED     0x38
#define LEFT_ALT_RELEASED    0xB8
#define LEFT_SHIFT_PRESSED   0x2A
#define LEFT_SHIFT_RELEASED  0xAA
#define RIGHT_SHIFT_PRESSED  0x36
#define RIGHT_SHIFT_RELEASED 0xB6
#define CAPS_LOCK_PRESSED    0x3A
#define NUM_LOCK_PRESSED     0x45

// Flags that indicate if a modifier key is pressed.
uint8_t caps  = 0;
uint8_t ctrl  = 0;
uint8_t shift = 0;
uint8_t alt   = 0;
uint8_t numl  = 0;

/* The table to convert scan code into ASCII code */
//If there is no correspoding scan code in the scan code set, put '\0'
char scan_code_table[SCAN_CODE_PRESS] = {
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	'\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'[', ']', '\n', '\0',
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' ,
	';', '\'', '`', '\0', '\\',
	'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',', '.', '/', '\0', '\0', '\0', ' ', '\0',
};

/* The table to translate scan code to ASCII code when shift is pressed */
char scan_code_table_shift[SCAN_CODE_PRESS] = {
	'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	'\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'{', '}', '\n', '\0',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' ,
	':', '\"', '~', '\0', '|',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	'<', '>', '?', '\0', '\0', '\0', ' ', '\0',
};

/* The table to translate scan code to ASCII code when caps is pressed */
char scan_code_table_caps[SCAN_CODE_PRESS] = {
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	'\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'[', ']', '\n', '\0',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' ,
	';', '\'', '`', '\0', '\\',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	',', '.', '/', '\0', '\0', '\0', ' ', '\0',
};

/* The table to translate scan code to ASCII code when shift and caps are pressed */
char scan_code_table_shift_caps[SCAN_CODE_PRESS] = {
	'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	'\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'{', '}', '\n', '\0',
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' ,
	':', '\"', '~', '\0', '|',
	'z', 'x', 'c', 'v', 'b', 'n', 'm',
	'<', '>', '?', '\0', '\0', '\0', ' ', '\0',
};

/*
*   key_init
*   initialize the keyboard's irq on PIC
*   input: None
*   output: None
*   side effect: PIC's 1st irq will be enabled. 
*/
void key_init(void) {
	/* The keyboard is connected to IR1 on the PIC */
	enable_irq(KEY_IRQ_NUM);
}


/*
*   key_handler
*   Handler function for keyboard:read what key was pressed on keyboard and print it on the screen
*   input: None
*   output: None
*   side effect: a character may appear on screen
*/
void key_handler(void) {
	cli();
	uint8_t scan_code;
	uint8_t ascii, i;

	/* Read from port to get the current scan code. */
    //the size of scan code is one byte
	scan_code = inb(KEY_BUF_PORT);
	
	/* Handle modifier keys */
	switch (scan_code) {
		case LEFT_CTRL_PRESSED:		ctrl = 1;		break;
		case LEFT_CTRL_RELEASED:	ctrl = 0;		break;
		case LEFT_ALT_PRESSED:		alt = 1;		break;
		case LEFT_ALT_RELEASED:		alt = 0;		break;
		case LEFT_SHIFT_PRESSED:	shift = 1;		break;
		case LEFT_SHIFT_RELEASED:	shift = 0;		break;
		case RIGHT_SHIFT_PRESSED:	shift = 1;		break;
		case RIGHT_SHIFT_RELEASED:	shift = 0;		break;
		case CAPS_LOCK_PRESSED:		caps = !caps;	break;
		case NUM_LOCK_PRESSED:		numl = !numl;	break;

		default:
            //get corresponding ascii for letters and numbers
            //invalid scan code
			if (scan_code >= SCAN_CODE_PRESS) {
                break;
            }
			if (shift && caps) {
				ascii = scan_code_table_shift_caps[scan_code];
			} 
            else if (shift) {
				ascii = scan_code_table_shift[scan_code];
			} 
            else if (caps) {
				ascii = scan_code_table_caps[scan_code];
			} 
            else {
				ascii = scan_code_table[scan_code];
			}
            putc(ascii);
            break;
	}

    //end of interrupt
	send_eoi(KEY_IRQ_NUM);
	sti();
}

