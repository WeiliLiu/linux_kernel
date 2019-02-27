#ifndef _SCHEDULING_H
#define _SCHEDULING_H

#include "terminal.h"

#define PIT_NUM 0
#define CHANNEL0 0x40
#define COMHZ 1193180
#define LOWBYTE 0xFF
#define SHIFTBYTE 8
#define COMREG 0x43
#define COMBYTE 0x36
#define FREQQ			1193200
#define BYTE_MASK 		0xFF
#define EIGHT			8
#define COMMAND_REG				0x43
#define COM_NUM 		0x36
#define CHANNEL_0 				0x40
#define TERM_SIZE 3
#define SEG 4

extern void pit_init(void);//initial pit
extern void pit_interrupt();//interrupt handler for pit
extern void context_switch();//do context switch when run different process

#endif
