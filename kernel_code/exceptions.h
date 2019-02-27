#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "syscall_wrapper.h"
#include "tests.h"




//below are the definition of the functions in exceptions.c
extern void Divide_Error(); /*Deal with divide_Error execption*/

extern void RESERVED(); /*Deal with reserved execption*/

extern void NMI_Interrupt();  /*Deal with NMI_Interrupt execption*/

extern void Breakpoint(); /*Deal with Breakpoint execption*/

extern void Overflow(); /*Deal with Overflow execption*/

extern void BOUND_Range_Exceeded(); /*Deal with BOUND_Range_Exceeded execption*/

extern void Invalid_Opcode(); /*Deal with Invalid_Opcode execption*/

extern void Device_Not_Available(); /*Deal with Device_Not_Available execption*/

extern void Double_Fault(); /*Deal with Double_Fault execption*/

extern void Coprocessor_Seg();  /*Deal with Coprocessor_Seg execption*/

extern void Invalid_TSS();  /*Deal with Invalid_TSS execption*/

extern void Segment_Not_Present();  /*Deal with Segment_Not_Present execption*/

extern void Stack_Seg_Fault();  /*Deal with Stack_Seg_Fault execption*/

extern void General_Protection(); /*Deal with General_Protection execption*/

extern void Page_Fault(); /*Deal with Page_Fault execption*/

extern void Floating_Error(); /*Deal with Floating_Error execption*/

extern void Alignment_Check();  /*Deal with Alignment_Check execption*/

extern void Machine_Check();  /*Deal with Machine_Check execption*/

extern void Floating_Exception(); /*Deal with Floating_Exception execption*/

#endif
