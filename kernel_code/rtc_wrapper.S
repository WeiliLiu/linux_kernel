.text
.globl rtc_wrapper
.align 4 /* maks sure each memory takes up 4 bytes */

/*rtc_wrapper
  Description: Interrupt wrapper function for rtc interrupt
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: handles rtc interrupts
*/
rtc_wrapper:
  pushal
  call rtc_interrupt  # call the function in rtc.h
  popal
  iret
