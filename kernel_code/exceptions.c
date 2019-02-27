#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "exceptions.h"
//The following files might refer to exceptions.c
#define haltnum 0xFF
#define SL1 26    /*string length of each exception*/
#define SL2 10
#define SL3 15
#define SL4 11
#define SL5 10
#define SL6 22
#define SL7 16
#define SL8 22
#define SL9 14
#define SL10 17
#define SL11 13
#define SL12 21
#define SL13 17
#define SL14 20
#define SL15 12
#define SL16 16
#define SL17 17
#define SL18 15
#define SL19 20
/*Divide_Error
  Description: Check for Divide_Error
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Divide_Error(){
  write(1, (void*)"Devide by Zero Exception\n", SL1);
  /*uint8_t ret = haltnum;
  halt(ret);*/                                    /*halt*/
  signal_delivery(0);
}

/*RESERVED
  Description: Check for RESERVED
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void RESERVED(){
  write(1, (void*)"Reserved\n", SL2);
  /*uint8_t ret = haltnum;
  halt(ret);*/                                      /*halt*/
  signal_delivery(1);
}

/*NMI_Interrupt
  Description: Check for NMI_Interrupt
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void NMI_Interrupt(){
  write(1, (void*)"NMI Interrupt\n", SL3);
  /*uint8_t ret = haltnum;
  halt(ret);*/
  signal_delivery(1);
}

/*Breakpoint
  Description: Check for Breakpoint
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Breakpoint(){
  write(1, (void*)"Breakpoint\n", SL4);
  //uint8_t ret = haltnum;
  //halt(ret);
  signal_delivery(1);
}

/*Overflow
  Description: Check for Overflow
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Overflow(){
  write(1, (void*)"Overflow\n", SL5);
  //uint8_t ret = haltnum;
  //halt(ret);                                    /*halt*/
  signal_delivery(1);
}

/*BOUND_Range_Exceeded
  Description: Check for BOUND_Range_Exceeded
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void BOUND_Range_Exceeded(){
  write(1, (void*)"Bound Range Exceeded\n", SL6);
  //uint8_t ret = haltnum;
  //halt(ret);                                        /*halt*/
  signal_delivery(1);
}

/*Invalid_Opcode
  Description: Check for Invalid_Opcode
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Invalid_Opcode(){
  write(1, (void*)"Invalid Opcode\n", SL7);
  //uint8_t ret = haltnum;
  //halt(ret);                                        /*halt*/
  signal_delivery(1);
}

/*Device_Not_Available
  Description: Check for Device_Not_Available
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Device_Not_Available(){
  write(1, (void*)"Device Not Available\n", SL8);
  //uint8_t ret = haltnum;
  //halt(ret);                                          /*halt*/
  signal_delivery(1);
}

/*Double_Fault
  Description: Check for Double_Fault
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Double_Fault(){
  write(1, (void*)"Double Fault\n", SL9);
  //uint8_t ret = haltnum;
  //halt(ret);                                        /*halt*/
  signal_delivery(1);
}

/*Coprocessor_Seg
  Description: Check for Coprocessor_Seg
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Coprocessor_Seg(){
  write(1, (void*)"Coprocessor Seg\n", SL10);
  //uint8_t ret = haltnum;
  //halt(ret);                                        /*halt*/
  signal_delivery(1);
}

/*Invalid_TSS
  Description: Check for Invalid_TSS
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Invalid_TSS(){
  write(1, (void*)"Invalid TSS\n", SL11);
  //uint8_t ret = haltnum;
  //halt(ret);                                          /*halt*/
  signal_delivery(1);
}

/*Segment_Not_Present
  Description: Check for Segment_Not_Present
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Segment_Not_Present(){
  write(1, (void*)"Segment Not Present\n", SL12);
  //uint8_t ret = haltnum;
  //halt(ret);                                              /*halt*/
  signal_delivery(1);
}

/*Stack_Seg_Fault
  Description: Check for Stack_Seg_Fault
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Stack_Seg_Fault(){
  write(1, (void*)"Stack Seg Fault\n", SL13);
  //uint8_t ret = haltnum;
  //halt(ret);                                              /*halt*/
  signal_delivery(1);
}

/*General_Protection
  Description: Check for General_Protection
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void General_Protection(){
  write(1, (void*)"General Protection\n", SL14);
  //uint8_t ret = haltnum;
  //halt(ret);                                                  /*halt*/
  signal_delivery(1);
}

/*Page_Fault
  Description: Check for Page_Fault
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Page_Fault(){
  write(1, (void*)"Page Fault\n", SL15);
  //uint8_t ret = haltnum;
  //halt(ret);                                                  /*halt*/
  signal_delivery(1);
}

/*Floating_Error
  Description: Check for Floating_Error
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Floating_Error(){
  write(1, (void*)"Floating Error\n", SL16);
  //uint8_t ret = haltnum;
  //halt(ret);                                            /*halt*/
  signal_delivery(1);
}

/*Alignment_Check
  Description: Check for Alignment_Check
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Alignment_Check(){
  write(1, (void*)"Alignment Check\n", SL17);
  //uint8_t ret = haltnum;
  //halt(ret);                                              /*halt*/
  signal_delivery(1);
}

/*Machine_Check
  Description: Check for Machine_Check
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Machine_Check(){
  write(1, (void*)"Machine Check\n", SL18);
  //uint8_t ret = haltnum;
  //halt(ret);                                            /*halt*/
  signal_delivery(1);
}

/*Floating_Exception
  Description: Check for Floating_Exception
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:sqush the user level program and return to shell
*/

void Floating_Exception(){
  write(1, (void*)"Floating Exception\n", SL19);
  //uint8_t ret = haltnum;
  //halt(ret);                                    /*halt*/
  signal_delivery(1);
}
