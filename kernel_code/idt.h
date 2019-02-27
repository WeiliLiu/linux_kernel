#ifndef _IDT_H
#define _IDT_H

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"

#define rtcindex 40
#define keyindex 33
#define pitindex 0x20
#define systemcallindex 0x80

void init_IDT(idt_desc_t*  idt);//refer to that of idt.c
void general_interrupt();
#endif
