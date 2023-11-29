#ifndef _SCHEDULER_H
#define _SCHEDULER_H


#define PIT_IO_PORT         0x43

//0x36 = 00110110 = channel 0(00)|access mode 3(11)|operating mode 3(011)|16-bit(0)
#define PIT_REG             0x36
#define CHANNEL_0           0x40

#define PIT_FREQ            11931       //The oscillator used by the PIT chip runs at (roughly) 1.193182 MHz. 
#define LOWER_MASK          0x00FF      //low byte         
#define HIGHER_MASK         0xFF00      //high byte

#define PIT_IRQ             0           //PIT has top priority


void PIT_init( void );
extern void pit_handler( void );


#endif
