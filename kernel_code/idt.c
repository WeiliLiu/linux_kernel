#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "exceptions.h"
#include "keyboard.h"
#include "idt.h"
#include "keyboard_wrapper.h"
#include "rtc_wrapper.h"
#include "paging.h"
#include "syscall_wrapper.h"
#include "pit_wrapper.h"

/*init_IDT
  Description: initialize IDT table
  INPUT: idt value
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: idt set
*/

void init_IDT(idt_desc_t*  idt){
  int i;
  //p156
  for (i=0;i<NUM_VEC;i++){
    idt[i].offset_15_00=0x00;
    idt[i].seg_selector=KERNEL_CS; //code_segment
    //set to interrupt descriptor first
    idt[i].reserved4=0x00;
    idt[i].reserved3=0x00;
    idt[i].reserved2=0x01;
    idt[i].reserved1=0x01;
    idt[i].size=0x01;
    idt[i].reserved0=0x00;
    idt[i].dpl=0x00;
    idt[i].present=0x00;
    idt[i].offset_31_16=0x00;
  }
  //set to execption handler
  idt[pitindex].present = 0x01;
  for(i = 0; i < keyindex+1; i++){
    idt[i].present = 0x01;
  }
  idt[rtcindex].present = 0x01;


/*  A segment selector to this “null
descriptor” does not generate an exception when loaded into a data-segment register (DS, ES,
FS, or GS)* P83*/


//System_CALL
  idt[systemcallindex].dpl = 3; /* privilege set to user level */
  idt[systemcallindex].present = 0x01;

//Fill IDT Entries
  //Exceptions, all of them could be traced in the file exceptions.c/h
  SET_IDT_ENTRY(idt[0],&Divide_Error);
  SET_IDT_ENTRY(idt[1],&RESERVED);
  SET_IDT_ENTRY(idt[2],&NMI_Interrupt);
  SET_IDT_ENTRY(idt[3], &Breakpoint);
  SET_IDT_ENTRY(idt[4],&Overflow);
  SET_IDT_ENTRY(idt[5],&BOUND_Range_Exceeded);
  SET_IDT_ENTRY(idt[6],&Invalid_Opcode);
  SET_IDT_ENTRY(idt[7],&Device_Not_Available);
  SET_IDT_ENTRY(idt[8],&Double_Fault);
  SET_IDT_ENTRY(idt[9],&Coprocessor_Seg);
  SET_IDT_ENTRY(idt[10],&Invalid_TSS);
  SET_IDT_ENTRY(idt[11],&Segment_Not_Present);
  SET_IDT_ENTRY(idt[12],&Stack_Seg_Fault);
  SET_IDT_ENTRY(idt[13],&General_Protection);
  SET_IDT_ENTRY(idt[14],&Page_Fault);
  SET_IDT_ENTRY(idt[16],&Floating_Error);
  SET_IDT_ENTRY(idt[17],&Alignment_Check);
  SET_IDT_ENTRY(idt[18],&Machine_Check);
  SET_IDT_ENTRY(idt[19],&Floating_Exception);
  //Interrupt
  SET_IDT_ENTRY(idt[pitindex], &pit_wrapper);    //PIT interrupt
  SET_IDT_ENTRY(idt[keyindex], &keyboard_wrapper);    //keyboard interrupt
  for(i = keyindex+1; i < rtcindex; i++){
    SET_IDT_ENTRY(idt[i], &general_interrupt);  //genreal interrupt
  }
  SET_IDT_ENTRY(idt[rtcindex], &rtc_wrapper);   //rtc interrupt
  //System call
  SET_IDT_ENTRY(idt[systemcallindex], &syscall_wrapper);  //general interrupt for rest
  for(i = rtcindex+1; i < systemcallindex; i++){
    SET_IDT_ENTRY(idt[i], &general_interrupt);
  }

  //load idtr
  lidt(idt_desc_ptr);
}

/*general interrupt
  Description: The interrupt
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: None
*/

void general_interrupt(){
  while(1){}
}
