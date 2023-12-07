#include "lib.h"
#include "scheduler.h"
#include "speaker.h"
#include "rtc.h"

//reference: https://wiki.osdev.org/PC_Speaker
void speaker_play(uint32_t nFrequence)
{
	uint32_t divide;
    uint8_t temp;

    if (nFrequence == 0) return;

    // Set the PIT to the desired frequency
    divide = 1193180 / nFrequence; 
    outb(0xb6, 0x43);
    outb((uint8_t) (divide), 0x42);
    outb((uint8_t) (divide >> 8), 0x42);

    // play the sound using the PC speaker
    temp = inb(0x61);
    if (temp != (temp | 3)) {
        outb(temp | 3, 0x61);
    }
}

void speaker_stop(void)
{
	uint8_t temp = inb(0x61) & 0xFC;
    outb(temp, 0x61);
}

void play_note(uint32_t freq){
	speaker_play(freq);
	rtc_read(0, 0, 0);
	speaker_stop();
}

void play_simple_music(){
	rtc_open(0);
	int32_t freq1 = 8 ;
	rtc_write(0, &freq1, 1);
	play_note(A3);
	play_note(A3);
	play_note(B3);
	play_note(C4);
	play_note(C4);
	play_note(B3);
	play_note(A3);
	play_note(G3);
	play_note(F3);
	play_note(F3);
	play_note(G3);
	play_note(A3);
	play_note(A3);
	play_note(G3);
	play_note(G3);

	rtc_close(0);
}
