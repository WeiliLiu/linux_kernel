.text
.globl pit_wrapper
.align 4 /* Make sure each memory takes up 4 bytes */


/*pit_wrapper
  Description: Interrupt wrapper function for pit interrupt
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: handles pit interrupts
*/
pit_wrapper:
  pushal
  pushfl
  call pit_interrupt   /* call the function in keyboard.c */
  popfl
  popal
  iret
