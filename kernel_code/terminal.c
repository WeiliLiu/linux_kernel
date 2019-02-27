#include "terminal.h"

terminal_t terminals[TERM_NUM];
int32_t displayed_terminal;
int32_t executing_terminal;
int32_t next_scheduled_terminal;

/*terminal_init
  Description: pit_init
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: Initialize any variables associated with terminal and enable
                terminal interrupts on the PIC
*/
void terminal_init(){
  //Initialize the process array to all available
  int i, j;
  for(i = 0; i < PROCESS_SIZE; i++){
    processes[i] = 0;
  }

  //Initialize the three types of terminal to 0
  displayed_terminal = 0;
  executing_terminal = 0;
  next_scheduled_terminal = 0;

  //Initialize the terminal 0
  terminals[terminal0].terminal_num = 0;
  terminals[terminal0].num_process_executing_on_terminal = 0;
  terminals[terminal0].cur_buf_size = 0;
  terminals[terminal0].old_buf_size = 0;
  terminals[terminal0].active_pcb = 0;
  terminals[terminal0].enter_flag = 0;

  //Initialize the terminal 1
  terminals[terminal1].terminal_num = 1;
  terminals[terminal1].num_process_executing_on_terminal = 0;
  terminals[terminal1].cur_buf_size = 0;
  terminals[terminal1].old_buf_size = 0;
  terminals[terminal1].active_pcb = 0;
  terminals[terminal1].enter_flag = 0;

  //Initialize the terminal 2
  terminals[terminal2].terminal_num = terminal2;
  terminals[terminal2].num_process_executing_on_terminal = 0;
  terminals[terminal2].cur_buf_size = 0;
  terminals[terminal2].old_buf_size = 0;
  terminals[terminal2].active_pcb = 0;
  terminals[terminal2].enter_flag = 0;

  for(i=0; i<PROCESS_SIZE; i++)
	{
		for(j=0; j< NUM_ROWS * NUM_COLS; j++) {
        *(uint8_t *)(FARPAGE + i * FOUR_KB	+ FOUR_KB + (j << 1)) = ' ';
        *(uint8_t *)(FARPAGE + i * FOUR_KB + FOUR_KB + (j << 1) + 1) = ATTRIB;
    }
  }

  user_video_page_table[VID_INDEX + INDEX_SEG] = VIDADDR | USER | WP;
}

/*terminal_open
  Description: Open terminal driver
  INPUT: None
  OUTPUT: None
  RETURN: 0 on success
  SIDE EFFECT: None
*/
int32_t terminal_open(){
  return 0;
}

/*terminal_close
  Description: closes terminal driver
  INPUT: None
  OUTPUT: None
  RETURN: 0 on success
  SIDE EFFECT: None
*/
int32_t terminal_close(){
  return 0;
}

/*terminal_read
  Description: read will return data from one line that has been terminated by pressing enter,
              or as much as fits in the buffer from one such line
  INPUT: fd - file descriptor, buf - input buffer to store the contents in the keyboard buffer,
        nbytes - number of bytes to copy
  OUTPUT: None
  RETURN: number of bytes read
  SIDE EFFECT: None
*/
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
  uint8_t* buffer = buf;

  while(terminals[executing_terminal].enter_flag == 0){} // Waiting for the user to press enter before doing any work

  if (nbytes == 0 || buf == NULL){return 0;}

  uint8_t i;
  /* Copy the contents in the keyboard buffer into the input buffer */
  for (i = 0; (i < nbytes - 1) && (i < terminals[executing_terminal].old_buf_size); i++){
    buffer[i] = terminals[executing_terminal].stored_old_buf[i];
  }

  buf = buffer;

  // clear the keyboard buffer and the enter flag
  terminals[executing_terminal].enter_flag = 0;

  return i;
}

/*terminal_write
  Description: nbytes of data or all the data in the input buffer, whichever is smaller,
              will be displayed to the screen
  INPUT: fd - file descriptor, buf - buffer with the data to print, nbytes - number of
              bytes to print
  OUTPUT: None
  RETURN: number of bytes printed
  SIDE EFFECT: characters stored in the input buffer will be displayed to the screen
*/
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
  cli();
  const uint8_t* buffer = buf;

  if (nbytes == 0 ){return 0;}
  if (buffer == NULL){return -1;}

  int32_t i;

  for(i = 0; i < nbytes; i++){
    if(buffer[i] != '\0'){
      if(executing_terminal == displayed_terminal){
        putc(buffer[i]);
        set_cursor();
      }else{
        putc_non_displayed(buffer[i]);
      }
    }
    // Update the cursor position after each character print
    //set_cursor();
  }
  sti();
  return nbytes;
}
