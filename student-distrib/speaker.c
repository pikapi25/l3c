#include "lib.h"
#include "scheduler.h"
#include "speaker.h"

//reference: https://wiki.osdev.org/PC_Speaker
void speaker_play(uint32_t nFrequence)
{
	uint32_t Div;
 	uint8_t tmp;
 
        //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outb(0xb6, 0x43);
 	outb((uint8_t) (Div), 0x42);
 	outb((uint8_t) (Div >> 8), 0x42);
 
        //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(tmp | 3, 0x61);
 	}
}

void speaker_stop(void)
{
	// turn off speaker (clear bits 0 and 1 of port 0x61)
	uint8_t temp = inb(0x61) & 0xFC;
	outb(temp, 0x61);
}
