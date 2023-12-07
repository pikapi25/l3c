#ifndef SPEAKER_H
#define SPEAKER_H

#define C3		131
#define D3		147
#define E3		165
#define F3		175
#define G3		196
#define A3		220
#define B3		247
#define C4		262
#define D4		294
#define E4		330

void speaker_play(uint32_t nFrequence);
void speaker_stop(void);
void play_note(uint32_t freq);
void play_simple_music();

#endif