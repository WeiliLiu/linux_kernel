#ifndef _RTC_H
#define _RTC_H
#include"i8259.h"
#include "terminal.h"

#define NMI_REGISTER_B 0x8B
#define RTC_PORT_ONE   0x70
#define RTC_PORT_TWO   0x71
#define VALUE_ORED     0x40
#define RTC_RATE       15
#define RATE_IN_RANGE  0x0F
#define NMI_REGISTER_A 0x8A
#define PREV_BITMASK   0xF0
#define RTC_IRQ_INTERRUPT_NUMBER  8
#define REGISTER_C     0x0C
/* define all possible frequency and rate*/
#define FREQ1  2
#define FREQ2  4
#define FREQ3  8
#define FREQ4  16
#define FREQ5  32
#define FREQ6  64
#define FREQ7  128
#define FREQ8  256
#define FREQ9  512
#define FREQ10 1024
#define RATE1  15
#define RATE2  14
#define RATE3  13
#define RATE4  12
#define RATE5  11
#define RATE6  10
#define RATE7  9
#define RATE8  8
#define RATE9  7
#define RATE10 6
#define terminal0 0
#define terminal1 1
#define terminal2 2

static volatile int32_t interrupt_flag;

extern void rtc_init();//refer to that in rtc.c
extern void rtc_interrupt();//the interrupt handler of rtc
extern int32_t rtc_close(int32_t fd);// no actual function, always returns 0
extern int32_t rtc_open(const uint8_t* filename);//no actual function, always returns 0
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) ;//after all interupts finished, return 0
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t n_bytes);//change the frequency of the real time clock

#endif
