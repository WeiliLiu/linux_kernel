#include "keyboard.h"

static int mode = 0;
static int ALT = unpressed;
static int SHIFT = unpressed;
static int CTRL = unpressed;

/*include the numbers and lower case alphabets*/
static uint8_t Scancode[scancodemode][scancodesize] = {{0,0,'1','2',
                                          '3','4','5','6',
                                          '7','8','9','0',
                                          '-','=',0,0,
                                          'q','w','e','r',
                                          't','y','u','i',
                                          'o','p','[',']',
                                          '\n',0,'a','s',
                                          'd','f','g','h',
                                          'j','k','l',';',
                                          '\'','`',0,'\\',
                                          'z','x','c','v',
                                          'b','n','m',',',
                                          '.','/',0,'*',
                                          0,' ',0},                                   /*original version*/
                                        {0,0,'!','@',
                                        '#','$','%','^',
                                        '&','*','(',')',
                                        '_','+',0,0,
                                        'Q','W','E','R',
                                        'T','Y','U','I',
                                        'O','P','{','}',
                                        '\n',0,'A','S',
                                        'D','F','G','H',
                                        'J','K','L',':',
                                        '"','~',0,'|',
                                        'Z','X','C','V',
                                        'B','N','M','<',
                                        '>','?',0,'*',
                                        0,' ',0
                                      },                                                /*original + shift*/
                                      {0,0,'!','@',
                                      '#','$','%','^',
                                      '&','*','(',')',
                                      '_','+',0,0,
                                      'q','w','e','r',
                                      't','y','u','i',
                                      'o','p','{','}',
                                      '\n',0,'a','s',
                                      'd','f','g','h',
                                      'j','k','l',':',
                                      '"','~',0,'|',
                                      'z','x','c','v',
                                      'b','n','m','<',
                                      '>','?',0,'*',
                                      0,' ',0                                           /*CAPSLOCK_ON + shift*/
                                    },{0,0,'1','2',
                                    '3','4','5','6',
                                    '7','8','9','0',
                                    '-','=',0,0,
                                    'Q','W','E','R',
                                    'T','Y','U','I',
                                    'O','P','[',']',
                                    '\n',0,'A','S',
                                    'D','F','G','H',
                                    'J','K','L',';',
                                    '\'','`',0,'\\',
                                    'Z','X','C','V',
                                    'B','N','M',',',
                                    '.','/',0,'*',
                                    0,' ',0}
                                  };                                                    /*CAPSLOCK_ON*/


/*keyboard_init
  Description: Initialize keyboard
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: Initialize any variables associated with keyboard and enable
                keyboard interrupts on the PIC
*/

void keyboard_init(){
  mode = 0;
  SHIFT = unpressed;
  CTRL = unpressed;
  ALT = unpressed;
  set_cursor();
  enable_irq(keyboard_irq_num);
}

/*getScancode
  Description: Get keyboard input
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: None
*/
void getScancode() {
  cli();
  uint8_t c = inb(DATAPORT);

  // Handling clearing screen with control-L
  if(c == LEFT_CTRL_ON){
    CTRL = pressed;
  }
  if(c == LEFT_CTRL_OFF){
    CTRL = unpressed;
  }

  switch(CTRL){
    case pressed:
      switch(c){
        case LETTER_L:
          clear();
          clear_buffer();
          break;
        case 0x2E:
          executing_terminal = displayed_terminal;
          signal_delivery(2);
      }
      break;
    case unpressed:
      break;
  }

  // Handling Capslock
  if(c == CAPSLOCK_ON){
    switch(mode){
      case ORIG_MODE:
        mode = CAP_MODE;
        break;
      case ORIG_SHIFT_MODE:
        mode = CAP_SHIFT_MODE;
        break;
      case CAP_SHIFT_MODE:
        mode = ORIG_MODE;
        break;
      case CAP_MODE:
        mode = ORIG_MODE;
        break;
    }
  }

  // Handling shift
  if(c == LEFT_SHIFT_ON || c == RIGHT_SHIFT_ON){
    SHIFT = pressed;
  }
  if(c == LEFT_SHIFT_OFF || c == RIGHT_SHIFT_OFF){
    SHIFT = unpressed;
  }

  if(CTRL == unpressed){
    switch(SHIFT){
      case pressed:
        switch(mode){
          case ORIG_MODE:
            mode = ORIG_SHIFT_MODE;
            break;
          case ORIG_SHIFT_MODE:
            mode = ORIG_SHIFT_MODE;
            break;
          case CAP_SHIFT_MODE:
            mode = CAP_SHIFT_MODE;
            break;
          case CAP_MODE:
            mode = CAP_SHIFT_MODE;
            break;
        }
        break;
      case unpressed:
        switch(mode){
          case ORIG_MODE:
            mode = ORIG_MODE;
            break;
          case ORIG_SHIFT_MODE:
            mode = ORIG_MODE;
            break;
          case CAP_SHIFT_MODE:
            mode = CAP_MODE;
            break;
          case CAP_MODE:
            mode = CAP_MODE;
            break;
        }
        break;
    }
  }

  // Handling pressing enter key
  if(c == ENTER){
    terminals[displayed_terminal].enter_flag = 1;
    int i;
    putc('\n');
    terminals[displayed_terminal].keyboard_buf[terminals[displayed_terminal].cur_buf_size] = '\n';
    terminals[displayed_terminal].cur_buf_size++;
    for(i = 0; i < terminals[displayed_terminal].cur_buf_size; i++){
      terminals[displayed_terminal].stored_old_buf[i] = terminals[displayed_terminal].keyboard_buf[i];
    }
    terminals[displayed_terminal].old_buf_size = terminals[displayed_terminal].cur_buf_size;
    set_cursor();
    terminals[displayed_terminal].cur_buf_size = 0;
  }

  if(c == LEFT_ALT_ON){
    ALT = pressed;
  }
  if(c == LEFT_ALT_OFF){
    ALT = unpressed;
  }

  if(ALT == pressed){
    if(c == F1){
      switch_active_terminal(terminal0);
      set_cursor();
    }
    if(c == F2){
      switch_active_terminal(terminal1);
      set_cursor();
    }
    if(c == F3){
      switch_active_terminal(terminal2);
      set_cursor();
    }
  }

  // fill the buffer
  if(Scancode[mode][c] != 0 && c < scancodesize && CTRL == unpressed){
    store_in_buffer(c);
  }else if(c == BACKSPACE){
    store_in_buffer(c);                           /*store the scan code into buffer*/
  }

  send_eoi(keyboard_irq_num);                    /*keyboard interrupt end*/
  sti();
}


/*store_in_buffer
  Description: store the key into buffer
  INPUT: scancode:c
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: Store the character corresponding to the scancode into the keybaord
              buffer
*/
void store_in_buffer(int c){
  if((terminals[displayed_terminal].cur_buf_size < (KEYBOARD_BUF_SIZE - minu)) || (terminals[displayed_terminal].cur_buf_size
    == (KEYBOARD_BUF_SIZE - minu))){
    if(c != BACKSPACE){
        if(c == ENTER){
          return;
        }else{
          terminals[displayed_terminal].keyboard_buf[terminals[displayed_terminal].cur_buf_size] = Scancode[mode][c];
          terminals[displayed_terminal].cur_buf_size++;
          out_key(c);                                             /*print out key with scancode c*/
          set_cursor();
        }
    }
  }

  if(c == BACKSPACE && terminals[displayed_terminal].cur_buf_size != 0){
    terminals[displayed_terminal].cur_buf_size --;
    backspace();
    set_cursor();
  }
}

/*clear_buffer
  Description: clear the buffer
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: Put the cursor to location (0,0) and clear the keyboard buffer
*/
void clear_buffer(){
  /* modified to not clear keyboard buffer for checkpoint 3*/
  reset_screen();
  set_cursor();
}


/*out_key
  Description: print out the key
  INPUT: scancode:c
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: Print the character corresponding to the scancode to the screen
*/
void out_key(int c){
  if(c < scancodesize){                   /*check if within right range*/
    putc(Scancode[mode][c]);
    set_cursor();
  }
}
