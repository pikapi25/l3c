#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "speaker.h"
#include "command_history.h"
#include "tab.h"
#include "signal.h"
#include "mouse.h"

//flags of modifier keys
uint8_t caps  = 0;
uint8_t ctrl  = 0;
uint8_t shift = 0;
uint8_t alt = 0;	//added in cp2 but not used
uint8_t numl  = 0;

//scan code table1
//If there is no correspoding ascii code in the scan code set, output '\0'
//the row of the table match with the row of US QWERTY keyboard
char scan_code_table[SCAN_CODE_PRESS] = {
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 
	'\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' , 
	';', '\'', '`', '\0', '\\', 
	'z', 'x', 'c', 'v', 'b', 'n', 'm', 
	',', '.', '/', '\0', '\0', '\0', ' ', '\0',
};

//the table used when caps is pressed
char caps_table[SCAN_CODE_PRESS] = {
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', '\0',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' ,
	';', '\'', '`', '\0', '\\',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	',', '.', '/', '\0', '\0', '\0', ' ', '\0',
};

//the table used when shift is pressed
char shift_table[SCAN_CODE_PRESS] = {
	'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	'\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'{', '}', '\n', '\0',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' ,
	':', '\"', '~', '\0', '|',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	'<', '>', '?', '\0', '\0', '\0', ' ', '\0',
};

//the table used when both shift and caps are pressed
char shift_and_caps_table[SCAN_CODE_PRESS] = {
	'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	'\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'{', '}', '\n', '\0',
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' ,
	':', '\"', '~', '\0', '|',
	'z', 'x', 'c', 'v', 'b', 'n', 'm',
	'<', '>', '?', '\0', '\0', '\0', ' ', '\0',
};

char keypad_table[KEYPAD_NUM]={
	'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

/*
*   key_init
*   initialize the keyboard's irq on PIC
*   input: None
*   output: None
*   side effect: PIC's IRQ1 is enabled. 
*/
void keyboard_init(void) {
	/* The keyboard is connected to IR1 on the PIC */
	enable_irq(KEY_IRQ_NUM);
}


/*
*   keyboard_handler
*   manage interrupt generated by keyboard
*   input: None
*   return printed ascii 
*   side effect: print the value of the pressed key on screen
*/
void keyboard_handler(void) {
	cli();
	uint8_t scan_code, ascii;
	int i;
	terminal_t* terminal = get_terminal();
	//get scan code from port
    //the size of scan code is one byte
	scan_code = inb(KEY_BUF_PORT);
	
	/* Handle modifier keys */
	switch (scan_code) {
		case LEFT_CTRL_PRESSED:		
			ctrl = 1;		
			break;
		case LEFT_CTRL_RELEASED:	
			ctrl = 0;		
			break;
		case LEFT_SHIFT_PRESSED:	
			shift = 1;		
			break;
		case LEFT_SHIFT_RELEASED:	
			shift = 0;		
			break;
		case RIGHT_SHIFT_PRESSED:	
			shift = 1;		
			break;
		case RIGHT_SHIFT_RELEASED:	
			shift = 0;		
			break;
		case LEFT_ALT_PRESSED:
			alt = 1;
			break;
		case LEFT_ALT_RELEASED:
			alt = 0;
			break;
		case CAPS_LOCK_PRESSED:		
			caps = !caps;	
			break;
		case NUM_LOCK_PRESSED:		
			numl = !numl;	
			break;
		//--------------------checkpoint5-----------------------
		/* terminal switch*/
		case F1_KEY:
			if (alt) {
				terminal_switch(TERM_ONE);
			}
			break;
		case F2_KEY:
			if (alt) {
				terminal_switch(TERM_TWO);
			}
			break;
		case F3_KEY:
			if (alt) {
				terminal_switch(TERM_THREE);
			}
			break;
		case UP_PRESSED:
			press_up();
			break;
		case DOWN_PRESSED:
			press_down();
			break;
		case TAB_PRESSED:
			press_tab();
			break;
		default:
            //get corresponding ascii for letters and numbers
            //invalid scan code, break

			//release piano key
			if ((numl||piano_button) && scan_code >= 0x80) {
				speaker_stop();
				break;
			}
			// else if((numl||piano_button) && scan_code>=0x47 &&scan_code<=0x53){
			// 	ascii = keypad_table[scan_code-0x47];
			// }
			else if (scan_code >= SCAN_CODE_PRESS) break;
			//if shift and caps are both pressed
			else if (shift && caps) {
				ascii = shift_and_caps_table[scan_code];
			}  
			//only caps is pressed
            else if (caps) {
				ascii = caps_table[scan_code];
			}
			//only srf is pressed
			else if (shift) {
				ascii = shift_table[scan_code];
			} 
			// //Number lock is unlocked
			// else if (numl) {
			// 	ascii = keypad_table[scan_code-KEYPAD_MASK];
			// }
			//no special keys are pressed
            else {
				ascii = scan_code_table[scan_code];
			}
			//signal
			if (ctrl && (ascii == 'c' || ascii == 'C')){
				send_signal(INTERRUPT);
				break;
			}
			//output in cp1
            // putc(ascii);
			// break;
			//--------------------checkpoint2-----------------------
			if ((numl||piano_button)&& scan_code>=0x02 &&scan_code<=0x0b) {
				piano(ascii);
				break;
			}
			//ctrl+l/L: clean the screen 
			if (ctrl && (ascii == 'l' || ascii == 'L')) {
				clear_redraw();						
				break;
			} 
			//ctrl+c/C: terminate current program
			else if (ctrl && (ascii == 'c' || ascii == 'C')) {			
				break;
			} 
			else if (ascii == '\n') {
				user_terminal_putc(ascii,1);
				if (terminal->readkey){
					terminal->kbd_buf[terminal->kbd_buf_count] = '\n';
					terminal->kbd_buf_count++;
					terminal->readkey = 0;							/* Set the "endline" flag */
				}else{
					reset_kbd_buf(curr_term_id);
				}
				
			} 
			else if (ascii == '\b') {
				if (terminal->kbd_buf_count > 0) {
					user_terminal_putc(ascii,1);
					terminal->kbd_buf_count--;
					terminal->kbd_buf[terminal->kbd_buf_count] = '\0';
				}
			} 
			else if (ascii == '\t') {
				for (i = 0; i < 2; i++) {
					if (terminal->kbd_buf_count < MAX_CHA_BUF - 1) {
						user_terminal_putc(' ',1);
						terminal->kbd_buf[terminal->kbd_buf_count] = ' ';
						terminal->kbd_buf_count++;
					}
				}
			} 
			else if (ascii != '\0') {
				if (terminal->kbd_buf_count < MAX_CHA_BUF - 1) {
					user_terminal_putc(ascii,1);
					terminal->kbd_buf[terminal->kbd_buf_count] = ascii;
					terminal->kbd_buf_count++;
				}
				//stop displaying when buffer is full
			}
	}

    //end of interrupt
	send_eoi(KEY_IRQ_NUM);
	//end of critical section
	sti();
}

void piano(uint8_t ascii){
	switch (ascii) {
		case '1':
			speaker_play(C3);	
			break;
		case '2':		
			speaker_play(D3);	
			break;
		case '3':		
			speaker_play(E3);	
			break;
		case '4':		
			speaker_play(F3);	
			break;
		case '5':		
			speaker_play(G3);	
			break;
		case '6':		
			speaker_play(A3);	
			break;
		case '7':	
			speaker_play(B3);	
			break;
		case '8':		
			speaker_play(C4);	
			break;
		case '9':		
			speaker_play(D4);	
			break;
		case '0':		
			speaker_play(E4);	
			break;
		default:
			return;
	}
}