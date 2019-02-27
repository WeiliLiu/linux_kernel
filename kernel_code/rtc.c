#include "rtc.h"
#include "lib.h"

/*rtc_init
  Description: Initialize rtc
  INPUT: None
  OUTPUT: None
  RETURN: None
*/

void rtc_init(){

  outb(NMI_REGISTER_B, RTC_PORT_ONE);		// select register B, and disable NMI
  char prev=inb(RTC_PORT_TWO);	// read the current value of register B
  outb(NMI_REGISTER_B, RTC_PORT_ONE);		// set the index again (a read will reset the index to register D)
  outb(prev | VALUE_ORED, RTC_PORT_TWO);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

  int rate = RTC_RATE;
  rate &= RATE_IN_RANGE;			// rate must be above 2 and not over 15
  outb(NMI_REGISTER_A, RTC_PORT_ONE);		// set index to register A, disable NMI
  prev=inb(RTC_PORT_TWO);	// get initial value of register A
  outb(NMI_REGISTER_A, RTC_PORT_ONE);		// reset index to A
  outb((prev & PREV_BITMASK) | rate, RTC_PORT_TWO); //write only our rate to A. Note, rate is the bottom 4 bits.

  enable_irq(RTC_IRQ_INTERRUPT_NUMBER);
}
/*
 * rtc_read
 *   DESCRIPTION: after all interupts finished, return 0
 *   INPUTS: three ignored inputs
 *   OUTPUTS: none
 *   RETURN VALUE: zero
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
	(void) fd;
  (void) buf;
  (void) nbytes;
  //sti();//set interrupt flag for reading
	terminals[executing_terminal].interrupt_flag = 0;
	while(1) {
		if (terminals[executing_terminal].interrupt_flag != 0)//went through the flag
    {
			break;
	  }
  }
  return 0;
}

/*
  Description: the interrupt handler of rtc
  INPUT: None
  OUTPUT: None
  RETURN: None
*/

void rtc_interrupt() {
  //test_interrupts();
  send_eoi(RTC_IRQ_INTERRUPT_NUMBER);     //gate for rtc
  outb(REGISTER_C, RTC_PORT_ONE);// set index to register C, disable NMI
  inb(RTC_PORT_TWO);
  terminals[terminal0].interrupt_flag = 1;
  terminals[terminal1].interrupt_flag = 1;
  terminals[terminal2].interrupt_flag = 1;
}
/*
 *   rtc_write
 *   DESCRIPTION: change the frequency of the real time clock
 *   INPUTS: pointer to the frequency variable
 *   OUTPUTS: return 0 on success, -1 on failure
 *   SIDE EFFECTS: changes the clock periodic interrupts frequency
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t n_bytes)
{
  (void) n_bytes;//release pointers
  (void) fd;
  unsigned char prev;
  int32_t rate = 0;
  int32_t frequency;
  // int i;
  // for (i = 0; i < NUM_PROC; i++)
  //   rtc_flag[i] =1;
  if ((int32_t*)buf)
  {
    frequency = *(int32_t*)buf;

    switch ( frequency ) {//set all different frequency
      case FREQ1:
        rate = RATE_IN_RANGE & RATE1;
        break;
      case FREQ2:
        rate = RATE_IN_RANGE & RATE2;
        break;
      case FREQ3:
        rate = RATE_IN_RANGE & RATE3;
        break;
      case FREQ4:
        rate = RATE_IN_RANGE & RATE4;
        break;
      case FREQ5:
        rate = RATE_IN_RANGE & RATE5;
        break;
      case FREQ6:
        rate = RATE_IN_RANGE & RATE6;
        break;
      case FREQ7:
       rate = RATE_IN_RANGE & RATE7;
        break;
      case FREQ8:
        rate = RATE_IN_RANGE & RATE8;
        break;
      case FREQ9:
        rate = RATE_IN_RANGE & RATE9;
        break;
      case FREQ10:
        rate = RATE_IN_RANGE & RATE10;
        break;

      default:
        return -1; //when we dont have a valid freq
        break;
      }
     cli(); //clear interrupts and change the frequency
     outb(NMI_REGISTER_A, RTC_PORT_ONE);// set index to register A, disable NMI
     prev = inb(RTC_PORT_TWO);// get initial value of register A
     outb(NMI_REGISTER_A, RTC_PORT_ONE);// reset index to A
     outb((prev & PREV_BITMASK) | rate, RTC_PORT_TWO);//write only our rate to A. Note, rate is the bottom 4 bits.
     sti();
     return 0; //success
  }
  return -1; //we were passed a null pointer!

}
/*
 *   rtc_open
 *   DESCRIPTION: no actual function, always returns 0
 *   INPUTS: the desired filename
 *   OUTPUTS: none
 *   RETURN VALUE:  0
 *
 */
int32_t rtc_open(const uint8_t* filename){
    (void) filename;
    return 0;
}

/*
 *   rtc_close
 *   DESCRIPTION: no actual function, always returns 0
 *   INPUTS: none
 *   OUTPUTS: 0
 *   RETURN VALUE:  0
 *   SIDE EFFECTS: none
 */
int32_t rtc_close(int32_t fd){
    return 0;
}
