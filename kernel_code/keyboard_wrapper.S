.text
.globl keyboard_wrapper
.align 4 /* Make sure each memory takes up 4 bytes */ 


/*keyboard_wrapper
  Description: Interrupt wrapper function for keyboard interrupt
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: handles keyboard interrupts
*/
keyboard_wrapper:
  pushal
  call getScancode   /* call the function in keyboard.c */
  popal
  iret
