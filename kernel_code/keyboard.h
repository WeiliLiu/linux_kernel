#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "paging.h"
#include "signal.h"

#define unpressed 0
#define pressed 1
#define keyboard_irq_num 0x1 //port for the keyboard
#define DATAPORT 0x60
#define LEFT_CTRL_ON 0x1D
#define LEFT_SHIFT_ON 0x2A
#define CAPSLOCK_ON 0x3A
#define LEFT_ALT_ON 0x38
#define RIGHT_SHIFT_ON 0x36
#define LEFT_CTRL_OFF 0x9D
#define LEFT_ALT_OFF 0xB8
#define LEFT_SHIFT_OFF 0xAA
#define RIGHT_SHIFT_OFF 0xB6
#define KEYBOARD_BUF_SIZE 128
#define ENTER 0x1C
#define LETTER_L 0x26
#define BACKSPACE 0x0E
#define F1 0x3B
#define F2 0x3C
#define F3 0x3D
#define ENDLINE 79
#define scancodesize 59
#define scancodemode 4
#define ORIG_MODE 0
#define ORIG_SHIFT_MODE 1
#define CAP_SHIFT_MODE 2
#define CAP_MODE 3
#define terminal0 0
#define terminal1 1
#define terminal2 2
#define minu 2

/*extern uint8_t keyboard_buf[KEYBOARD_BUF_SIZE];
extern uint8_t stored_old_buf[KEYBOARD_BUF_SIZE];
extern int cur_buf_size;
extern int old_buf_size;*/
volatile int enter_flag;

extern void keyboard_init();//refer to that of the keyboard.c
extern void getScancode();  //handle keyboard interrupt
extern void store_in_buffer(int c);//store key into buffer
extern void out_key(int c);//print key
extern void clear_buffer();//clear the buffer


#endif
