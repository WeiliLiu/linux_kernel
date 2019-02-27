#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"
#include "syscall.h"
#include "paging.h"
#include "pcb.h"

#define ENDLINE 79
#define KEYBOARD_BUF_SIZE 128
#define NUM_COLS    80
#define NUM_ROWS    25
#define ATTRIB      0x7
#define PROCESS_SIZE 6
#define terminal0 0
#define terminal1 1
#define terminal2 2
#define TERM_NUM 3
#define VID_INDEX 0xB8

//terminal struct
typedef struct terminal{
  pcb_t* active_pcb;
  int32_t screen_x;
  int32_t screen_y;
  int32_t terminal_num;
  int32_t num_process_executing_on_terminal;
  uint8_t keyboard_buf[KEYBOARD_BUF_SIZE];
  uint8_t stored_old_buf[KEYBOARD_BUF_SIZE];
  int32_t cur_buf_size;
  int32_t old_buf_size;
  uint32_t esp;
  uint32_t ebp;
  uint32_t enter_flag;
  uint32_t interrupt_flag;
}terminal_t;

extern terminal_t terminals[TERM_NUM];
extern int32_t displayed_terminal;
extern int32_t executing_terminal;
extern int32_t next_scheduled_terminal;

extern void terminal_init();//initialize terminal
/* Open terminal driver */
extern int32_t terminal_open();
/* Close terminal driver */
extern int32_t terminal_close();
/* read will return data from one line that has been terminated by pressing enter,
or as much as fits in the buffer from one such line*/
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
/* nbytes of data or all the data in the input buffer, whichever is smaller,
will be displayed to the screen */
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
