/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/*i8259_init
  Description: Initialize the 8259 PIC
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: mask interrupt
*/

/* Initialize the 8259 PIC */
void i8259_init(void) {
  //Mask the lines
  outb(master_mask, MASTER_DATA_PORT);
  outb(slave_mask, SLAVE_DATA_PORT);

  //initialize master
  outb(ICW1, MASTER_8259_PORT);
  outb(MASTER_8259_PORT, MASTER_DATA_PORT);
  outb(ICW3_MASTER, MASTER_DATA_PORT);
  outb(ICW4, MASTER_DATA_PORT);

  //initialize slave
  outb(ICW1, SLAVE_8259_PORT);
  outb(ICW2_SLAVE, SLAVE_DATA_PORT);
  outb(ICW3_SLAVE, SLAVE_DATA_PORT);
  outb(ICW4, SLAVE_DATA_PORT);

  //Enable master and slave port so that they won't get changed during the initization
  outb(master_mask, MASTER_DATA_PORT);
  outb(slave_mask, SLAVE_DATA_PORT);
  enable_irq(NMILINE);

}

/*enable_irq
  Description: Enable (unmask) the specified IRQ
  INPUT: the irq number to be enabled
  OUTPUT: None
  RETURN: None
  SIDE EFFECT:enable the irq
*/

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
  uint16_t port;
  uint8_t value;

  if(irq_num < MASTERBOUND){            //master enable_irq
    port = MASTER_DATA_PORT;
  } else{                               //slave enable_irq
    port = SLAVE_DATA_PORT;
    irq_num -= MASTERBOUND;
  }
  value = inb(port) & ~(1 << irq_num);//the operation
  outb(value, port);
}

/*disable_irq
  Description: Disable (mask) the specified IRQ
  INPUT: the irq number to be disabled
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: disable irq
*/

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
  uint16_t port;
  uint8_t value;

  if(irq_num < MASTERBOUND){    //master disable_irq
    port = MASTER_DATA_PORT;
  } else{                       //slave disable_irq
    port = SLAVE_DATA_PORT;
    irq_num -= MASTERBOUND;
  }

  value = inb(port) | (1 << irq_num);//the operation
  outb(value, port);
}

/*send_eoi
  Description: Send end-of-interrupt signal for the specified IRQ
  INPUT: the irq need to be sent
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: NONE
*/

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
  unsigned long interrupt_command = irq_num | EOI;
  if(irq_num >= MASTERBOUND){
    outb((irq_num - MASTERBOUND) | EOI, SLAVE_8259_PORT); //slave send_eoi
    outb(ICW3_SLAVE | EOI, MASTER_8259_PORT);
  }
  outb(interrupt_command, MASTER_8259_PORT);  //master send_eoi
}
