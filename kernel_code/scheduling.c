#include "scheduling.h"
/*pit_init
  Description: pit_init
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: Initialize any variables associated with pit and enable
                pit interrupts on the PIC
*/
void pit_init(void) {
    /*set frequency*/
    outb(COM_NUM, COMMAND_REG);
    outb(FREQQ & BYTE_MASK, CHANNEL_0);
    outb(FREQQ >> EIGHT, CHANNEL_0);

    /*Enable pit irq with number 0*/
    enable_irq(PIT_NUM);
    return;
}

/*pit_interrupt
  Description: interrupt handler for pit
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: switch context and disable pit
*/
void pit_interrupt(){
  cli();
  disable_irq(PIT_NUM);
  next_scheduled_terminal = (executing_terminal + 1) % TERM_SIZE;
  send_eoi(PIT_NUM);
  context_switch();
  sti();
}

/*context_switch
  Description: Do context switch if change to another processes
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: enable irq for pit
*/
void context_switch(){
  cli();
  if(terminals[0].num_process_executing_on_terminal == 0)
	{
		sti();
    enable_irq(PIT_NUM);
		return;
	}

	/* Return if the required switch is meaningless */
	if(next_scheduled_terminal == executing_terminal)
	{
		sti();
    enable_irq(PIT_NUM);
		return;
	}

  /* get esp */
	asm volatile(
		"movl %%ebp, %0 	\n" // first output
    "movl %%esp, %1 	\n" // second output
		: "=g"(terminals[executing_terminal].ebp),"=g"(terminals[executing_terminal].esp)
		:
		: "memory"
		);

  if(terminals[next_scheduled_terminal].num_process_executing_on_terminal == 0){
    int32_t temp_term = executing_terminal;
		executing_terminal = next_scheduled_terminal;
    enable_irq(PIT_NUM);
    sti();
    execute((uint8_t*)"shell");
    executing_terminal = temp_term;
		displayed_terminal = temp_term;
		return;
  }

  int32_t next = terminals[next_scheduled_terminal].active_pcb->process_num;

  //remap for process switch
  process_map(next);

  tss.ss0 = KERNEL_DS;
	tss.esp0 = (uint32_t)(EIGHT_MB - EIGHT_KB* next - SEG);

  executing_terminal = next_scheduled_terminal;

  /* Switch the Stack */
	/* restore esp */
	asm volatile(
			"movl %0, %%esp 	\n"
      "movl %1, %%ebp	\n"
			:
			:"g"(terminals[next_scheduled_terminal].esp),"g"(terminals[next_scheduled_terminal].ebp)
			:"memory"
			);

  enable_irq(PIT_NUM);
  return;
}
