#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "signal.h"
// #include "speaker.h"

//flags of modifier keys
uint8_t caps  = 0;
uint8_t ctrl  = 0;
uint8_t shift = 0;
uint8_t alt = 0;	//added in cp2 but not used
// uint8_t numl  = 0;

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

// char keypad_table[KEYPAD_NUM]={
// 	'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
// };

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
		// case NUM_LOCK_PRESSED:		
		// 	numl = !numl;	
		// 	break;
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

		default:
            //get corresponding ascii for letters and numbers
            //invalid scan code, break
			//temp handler in checkpoint1 and checkpoint2
			// if (scan_code >= 0x80) {//release
			// 	speaker_stop();
			// 	break;
			// }
			if (scan_code >= SCAN_CODE_PRESS) break;
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
			// if (numl) {
			// 	switch (ascii) {
			// 		case 'z':	case 'Z':	speaker_play(FREQ_C3);	break;
			// 		case 's':	case 'S':	speaker_play(FREQ_CS3);	break;
			// 		case 'x':	case 'X':	speaker_play(FREQ_D3);	break;
			// 		case 'd':	case 'D':	speaker_play(FREQ_DS3);	break;
			// 		case 'c':	case 'C':	speaker_play(FREQ_E3);	break;
			// 		case 'v':	case 'V':	speaker_play(FREQ_F3);	break;
			// 		case 'g':	case 'G':	speaker_play(FREQ_FS3);	break;
			// 		case 'b':	case 'B':	speaker_play(FREQ_G3);	break;
			// 		case 'h':	case 'H':	speaker_play(FREQ_GS3);	break;
			// 		case 'n':	case 'N':	speaker_play(FREQ_A3);	break;
			// 		case 'j':	case 'J':	speaker_play(FREQ_AS3);	break;
			// 		case 'm':	case 'M':	speaker_play(FREQ_B3);	break;
			// 		case ',':	case '<':	speaker_play(FREQ_C4);	break;
			// 		case 'l':	case 'L':	speaker_play(FREQ_CS4);	break;
			// 		case '.':	case '>':	speaker_play(FREQ_D4);	break;
			// 		case ';':	case ':':	speaker_play(FREQ_DS4);	break;
			// 		case '/':	case '?':	speaker_play(FREQ_E4);	break;
			// 		case 'q':	case 'Q':	speaker_play(FREQ_C4);	break;
			// 		case '2':				speaker_play(FREQ_CS4);	break;
			// 		case 'w':	case 'W':	speaker_play(FREQ_D4);	break;
			// 		case '3':				speaker_play(FREQ_DS4);	break;
			// 		case 'e':	case 'E':	speaker_play(FREQ_E4);	break;
			// 		case 'r':	case 'R':	speaker_play(FREQ_F4);	break;
			// 		case '5':				speaker_play(FREQ_FS4);	break;
			// 		case 't':	case 'T':	speaker_play(FREQ_G4);	break;
			// 		case '6':				speaker_play(FREQ_GS4);	break;
			// 		case 'y':	case 'Y':	speaker_play(FREQ_A4);	break;
			// 		case '7':				speaker_play(FREQ_AS4);	break;
			// 		case 'u':	case 'U':	speaker_play(FREQ_B4);	break;
			// 		case 'i':	case 'I':	speaker_play(FREQ_C5);	break;
			// 		case '9':				speaker_play(FREQ_CS5);	break;
			// 		case 'o':	case 'O':	speaker_play(FREQ_D5);	break;
			// 		case '0':				speaker_play(FREQ_DS5);	break;
			// 		case 'p':	case 'P':	speaker_play(FREQ_E5);	break;
			// 		case '[':	case '{':	speaker_play(FREQ_F5);	break;
			// 		case '=':	case '+':	speaker_play(FREQ_FS5);	break;
			// 		case ']':	case '}':	speaker_play(FREQ_G5);	break;
			// 	}
			// 	break;
			// }
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

