#include "syscall.h"

#define UPPB 7
#define LOWB 0
#define haltnum 0xFF
#define haltret 256
#define ARGLEN 128
#define MAGIC1 0x7f
#define MAGIC2 0x45
#define MAGIC3 0x4c
#define MAGIC4 0x46
#define bufferlen 30
#define entrynum1 27
#define entrynum2 26
#define entrynum3 25
#define entrynum4 24
#define clearmask 0x00000000
#define BYTESHIFT1 8
#define BYTESHIFT2 (BYTESHIFT1 * 2)
#define BYTESHIFT3 (BYTESHIFT1 * 3)

uint32_t processes[MAX_NUM_PROCESS]; // An array containing the number of active processes
uint32_t num_process_executing = 0;
uint8_t fname[FILENAME_LEN];
uint8_t arg[ARGLEN];
uint32_t arg_length;

/*sys_halt
  Description: Halt the current program
  INPUT: status - argument from BX
  OUTPUT: 256 if exception occurs; 32 bits status otherwise
  RETURN: 0
  SIDE EFFECT: Restore to the parent program
*/
int32_t sys_halt(uint8_t status){
  int i;



  /////////////////////////////////////////////////////////////////////////////
  //Clearing the process control block of the current halting process
  /////////////////////////////////////////////////////////////////////////////
  pcb_t* curr_pcb = terminals[executing_terminal].active_pcb;

  /*close files*/
  for(i = REG_FILE; i < UPPB; i++){
    sys_close(i);
  }


  // Decrement the number of process executing
  num_process_executing--;
  terminals[executing_terminal].num_process_executing_on_terminal--;
  processes[curr_pcb->process_num] = 0;


  /////////////////////////////////////////////////////////////////////////////
  //Relaunch shell if the halting process if the last process
  /////////////////////////////////////////////////////////////////////////////
  if(curr_pcb->parent_pcb == 0){
    sys_execute((uint8_t*)"shell");
  }


  /////////////////////////////////////////////////////////////////////////////
  //Restore process control block for the parent process
  /////////////////////////////////////////////////////////////////////////////
  pcb_t* pcb_ptr = (pcb_t*)(curr_pcb->parent_pcb);
  terminals[executing_terminal].active_pcb = pcb_ptr;

  //restore paging for parent
  process_map(pcb_ptr->process_num);



  /////////////////////////////////////////////////////////////////////////////
  //Prepare for context switch
  /////////////////////////////////////////////////////////////////////////////
  tss.ss0 = KERNEL_DS;
  tss.esp0 = curr_pcb->curr_esp0;                                           /*set tss*/

  // Set up return value
  uint32_t ret;
  if(status == haltnum){
    ret = haltret;
  } else{
    ret = (uint32_t)status;
  }

  asm volatile("movl %0, %%eax\n" // First input
               "movl %1, %%esp\n" // Second input
               "movl %2, %%ebp\n" // Third input
               "jmp RETURN_FROM_IRET\n"
               :
               :"r"((uint32_t)ret), "r"(curr_pcb->esp), "r"(curr_pcb->ebp)              /*linkage to execute*/
               :"%eax"
               );


  return 0;
}

/*parse_command
  Description: get the filename and argument
  INPUT: command - the command from the sys_execute,
         fnmae_buf - a buffer stores FILENAME
         arg_buf - a buffer stores argument
  OUTPUT: the fname buf
  RETURN: Nothing
  SIDE EFFECT: Change the fname_buf and arg_buf's value
*/
void parse_command(const uint8_t* command, uint8_t* fname_buf, uint8_t* arg_buf){
  int command_length = strlen((int8_t*)command);
  int i, fname_length;
  fname_length = 0;
  i = 0;
  for(i = 0; i < command_length && i < FILENAME_LEN; i++){
    if(command[i] == ' ' || command[i] == '\0')                   /*get filename length*/
      break;
  }
  fname_length = i;
  for(i = 0; i < fname_length; i++){
    if(command[i] == '\0'){
      fname_buf[i] = '\0';                                          /*set filename*/
      break;
    }
    fname_buf[i] = command[i];
  }
  for(i = fname_length; i < FILENAME_LEN; i++){
    fname_buf[i] = '\0';                                   /*clear the rest in filename buffer*/
  }

  arg_length = 0;
  i = fname_length + 1;                  /*i to the first bit after the space*/
  while(i<command_length){
    if(command[i] == '\0'){
      arg_buf[i - fname_length - 1] = '\0';
      break;
    }else{
      arg_buf[i - fname_length - 1] = command[i];                 /*set arg buffer*/
      arg_length++;
    }
    i++;
  }
  for(i = (command_length-fname_length-1); i < ARGLEN; i++){
    arg_buf[i] = '\0';                          /*clear the rest in arg buffer*/
  }
  return;
}

/*loader
  Description: user level load
  INPUT: buf the content buffer
  OUTPUT: the virtual address
  RETURN: first_instruction: the virtual address
  SIDE EFFECT: none
*/
uint32_t loader(uint8_t* buf){
  uint32_t first_instruction = clearmask;
  first_instruction = first_instruction | ((buf[entrynum1] | clearmask) << BYTESHIFT3);
  first_instruction = first_instruction | ((buf[entrynum2] | clearmask) << BYTESHIFT2);
  first_instruction = first_instruction | ((buf[entrynum3] | clearmask) << BYTESHIFT1);
  first_instruction |= buf[entrynum4];
  return first_instruction;
}

/*executable_check
  Description: check if a given file is an executable file
  INPUT: buf the content buffer, directory_idx - the index of the file in the directory
  OUTPUT: none
  RETURN: 0 - success, -1 - failure
  SIDE EFFECT: none
*/
int32_t executable_check(uint8_t* buf, int32_t directory_idx){
  read_data(boot_block_ptr->direntries[directory_idx].inode_num, 0, buf, bufferlen);
  if(buf[FIRST_WORD] != MAGIC1){
    num_process_executing--;
    return -1;
  }
  if(buf[SECOND_WORD] != MAGIC2){
    num_process_executing--;
    return -1;
  }
  if(buf[THIRD_WORD] != MAGIC3){
    num_process_executing--;
    return -1;
  }
  if(buf[FOURTH_WORD] != MAGIC4){
    num_process_executing--;
    return -1;
  }
  return 0;
}

/*open_io
  Description: open io function for a given process
  INPUT: none
  OUTPUT: none
  RETURN: none
  SIDE EFFECT: none
*/
void open_io(){
  // opening stdin in fd
  terminals[executing_terminal].active_pcb->f_desc[0].flag = 0;
  terminals[executing_terminal].active_pcb->f_desc[0].file_position = 0;
  terminals[executing_terminal].active_pcb->f_desc[0].inode = 0;
  terminals[executing_terminal].active_pcb->f_desc[0].ft_ptr.open = &terminal_open;
  terminals[executing_terminal].active_pcb->f_desc[0].ft_ptr.read = &terminal_read;
  terminals[executing_terminal].active_pcb->f_desc[0].ft_ptr.write = &terminal_write;
  terminals[executing_terminal].active_pcb->f_desc[0].ft_ptr.close = &terminal_close;
  // opening stdout in fd
  terminals[executing_terminal].active_pcb->f_desc[1].flag = 0;
  terminals[executing_terminal].active_pcb->f_desc[1].file_position = 0;
  terminals[executing_terminal].active_pcb->f_desc[1].inode = 0;
  terminals[executing_terminal].active_pcb->f_desc[1].ft_ptr.open = &terminal_open;
  terminals[executing_terminal].active_pcb->f_desc[1].ft_ptr.read = &terminal_read;
  terminals[executing_terminal].active_pcb->f_desc[1].ft_ptr.write = &terminal_write;
  terminals[executing_terminal].active_pcb->f_desc[1].ft_ptr.close = &terminal_close;
}

/*sys_execute
  Description: Execute the current program
  INPUT: command - contain a filename in front of a " ", rest of it are arguments
  OUTPUT: 0-256 decided by sys_halt
  RETURN: 0 if success; -1 if fail
  SIDE EFFECT: Restore to the parent program
*/
int32_t sys_execute(const uint8_t* command){
  /////////////////////////////////////////////////////////////////////////////
  //Error Checking
  /////////////////////////////////////////////////////////////////////////////

  // If there are already 6 processes running, return error
  if(num_process_executing == MAX_NUM_PROCESS){
    return -1;
  }

  /* Null pointer is passed in */
  if(command == NULL){
    return -1;
  }

  //Parse command
  parse_command(command, fname, arg);
  //check if filename exists
  int i;
  int file_found = 0;
  int directory_idx;
  for(i = 0; i < boot_block_ptr->dir_count; i++){
    if(strncmp((int8_t*)fname, boot_block_ptr->direntries[i].filename, FILENAME_LEN) == 0){
      file_found = 1;
      directory_idx = i;
      break;
    }
  }
  // if the file exist, check if the file is an executable
  uint8_t buf[bufferlen];
  if(file_found == 0){
    return -1; // return -1 if the named file does not exist in the file directory
  } else{
    if(executable_check(buf, directory_idx) == -1){
      return -1;
    }
  }



  /////////////////////////////////////////////////////////////////////////////
  //Seaching for available process number
  /////////////////////////////////////////////////////////////////////////////
  uint32_t process_num;
  for(i = 0; i < MAX_NUM_PROCESS; i++){
    if(processes[i] == 0){
      process_num = i;
      processes[i] = 1; //Mark it as unavailable
      break;  // exit the loop when available process number is found
    }
  }
  num_process_executing++; // Increment the number of processes executing
  terminals[executing_terminal].num_process_executing_on_terminal++;


  /////////////////////////////////////////////////////////////////////////////
  //Set up new process paging
  /////////////////////////////////////////////////////////////////////////////
  process_map(process_num);



  /////////////////////////////////////////////////////////////////////////////
  //Load the location of the first instruction
  /////////////////////////////////////////////////////////////////////////////
  uint32_t first_instruction = loader(buf);


  /////////////////////////////////////////////////////////////////////////////
  //Copy the content of the file to memory 0x08048000
  /////////////////////////////////////////////////////////////////////////////
  inode_t* cur_inode= (inode_t*)((uint32_t)boot_block_ptr +
  (boot_block_ptr->direntries[directory_idx].inode_num + 1) * BLOCK_SIZE);
  uint32_t length = cur_inode->length;
  read_data(boot_block_ptr->direntries[directory_idx].inode_num, 0, (uint8_t*)VIRTUALADDR, length);



  /////////////////////////////////////////////////////////////////////////////
  //Set up the process control block for the new process
  /////////////////////////////////////////////////////////////////////////////
  pcb_t* temp_parent_pcb = terminals[executing_terminal].active_pcb;
  terminals[executing_terminal].active_pcb = (pcb_t*)(EIGHT_MB - (process_num + 1) * EIGHT_KB);

  for(i = FIRST_REG_FILE; i < LAST_REG_FILE; i++){
    terminals[executing_terminal].active_pcb->f_desc[i].inode = 0;
    terminals[executing_terminal].active_pcb->f_desc[i].file_position = 0;
    terminals[executing_terminal].active_pcb->f_desc[i].flag = 1;
  }

  open_io();

  terminals[executing_terminal].active_pcb->process_num = process_num;
  if(terminals[executing_terminal].num_process_executing_on_terminal <= 1){
    terminals[executing_terminal].active_pcb->parent_pcb = (uint32_t*)0;
  }else{
    terminals[executing_terminal].active_pcb->parent_pcb = (uint32_t*)temp_parent_pcb;
    /*set parent pcb*/
  }

  asm volatile("			\n\
          movl %%ebp, %%eax 	\n\
          movl %%esp, %%ebx 	\n\
        "
        :"=a"(terminals[executing_terminal].active_pcb->ebp), "=b"(terminals[executing_terminal].active_pcb->esp));
        /*store pcb location in stack*/

  terminals[executing_terminal].active_pcb->curr_esp0 = tss.esp0;



  /////////////////////////////////////////////////////////////////////////////
  //Prepare for context switch
  /////////////////////////////////////////////////////////////////////////////
  uint32_t stack_point = VIRSTART + FOUR_MB;          /*set starting virtual addr*/

  // prepare for context swtich
  tss.ss0 = KERNEL_DS;
  tss.esp0 = EIGHT_MB - EIGHT_KB * process_num;

  asm volatile(
               "cli;"
               "mov %0, %%ax;" //First input
               "mov %%ax, %%ds;"
               "pushl %1;" //Second input
               "pushl %2;" //Third input
               "pushfl;"
               "popl %%edx;"
               "orl $0x200, %%edx;" // Setting the IF flag on EFLAGS register
               "pushl %%edx;"
               "pushl %3;" //Fourth input
               "pushl %4;" //Fifth input
               "iret;"
               "RETURN_FROM_IRET:;"
               "LEAVE;"
               "RET;"
               :
               :"g"(USER_DS), "r"(USER_DS), "r"(stack_point), "r"(USER_CS), "r"(first_instruction)	/* input */
               :"%edx","%eax"
               );

  return 0;
}


/*sys_read
  Description: Read the current program
  INPUT: fd - file descriptor number
         buf - a buffer contains the data need to read
         nbytes - length of byte need to read
  OUTPUT: the data of the file
  RETURN: -1 if failed
  SIDE EFFECT: None
*/
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes){
  if(fd < LOWB || fd > UPPB || fd == 1){
    return -1;
  }
  if(fd >= FIRST_REG_FILE &&
    fd <= UPPB && terminals[executing_terminal].active_pcb->f_desc[fd].flag == 1){
    return -1;
  }
  return terminals[executing_terminal].active_pcb->f_desc[fd].ft_ptr.read(fd, buf, nbytes);
}

/*sys_write
  Description: Read the current program
  INPUT: fd - file descriptor number
         buf - a buffer contains the data need to write
         nbytes - length of byte need to write
  OUTPUT: the data of the file
  RETURN: -1 if failed
  SIDE EFFECT: None
*/
int32_t sys_write(int32_t fd, const void* buf, int32_t nbytes){
  if(fd < LOWB || fd > UPPB || fd == 0){
    return -1;
  }
  if(fd >= FIRST_REG_FILE &&
    fd <= UPPB && terminals[executing_terminal].active_pcb->f_desc[fd].flag == 1){
    return -1;
  }
  return terminals[executing_terminal].active_pcb->f_desc[fd].ft_ptr.write(fd, buf, nbytes);
}

/*sys_open
  Description: Open the current program
  INPUT: fname - filename needs to open
  OUTPUT: the file index
  RETURN: 0-7
  SIDE EFFECT: None
*/
int32_t sys_open(const uint8_t* fname){
  int i;
  int file_found = 0;
  int free_fd = 0;
  int directory_idx, fd_idx;
  for(i = 0; i < boot_block_ptr->dir_count; i++){
    if(strncmp((int8_t*)fname, boot_block_ptr->direntries[i].filename, FILENAME_LEN) == 0){
      file_found = 1;
      directory_idx = i;
      break;
    }
  }

  if(file_found == 0){
    return -1; // return -1 if the named file does not exist in the file directory
  }else{
    for(i = FIRST_REG_FILE; i < LAST_REG_FILE; i++){
      if(terminals[executing_terminal].active_pcb->f_desc[i].flag == 1){
        free_fd = 1;
        fd_idx = i;
        terminals[executing_terminal].active_pcb->f_desc[fd_idx].flag = 0; // change the flag of the file descriptor to in-use
        break;
      }
    }
    // check if free file descriptor is available
    if(free_fd == 0){
      return -1;
    }else{
      // initialize the position to start of file when file is first opened
      terminals[executing_terminal].active_pcb->f_desc[fd_idx].file_position = 0;
      // Set up inode number and fill file operations jump table associated with the correct file type
      switch(boot_block_ptr->direntries[directory_idx].filetype){
        case RTC_FILE:
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.open = &rtc_open;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.read = &rtc_read;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.write = &rtc_write;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.close = &rtc_close;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].inode = 0;
          break;
        case DIR_FILE:
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.open = &dir_open;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.read = &dir_read;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.write = &dir_write;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.close = &dir_close;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].inode = 0;
          break;
        case REG_FILE:
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.open = &file_open;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.read = &file_read;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.write = &file_write;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].ft_ptr.close = &file_close;
          terminals[executing_terminal].active_pcb->f_desc[fd_idx].inode
          = boot_block_ptr->direntries[directory_idx].inode_num;
      }
    }
  }
  return fd_idx;
}

/*sys_close
  Description: Open the current file
  INPUT: fname - filename needs to open
  OUTPUT: 0
  RETURN: 0 if success; -1 if failed
  SIDE EFFECT: None
*/
int32_t sys_close(int32_t fd){
  if(fd < FIRST_REG_FILE || fd >= LAST_REG_FILE){
    return -1;
  }
  if(terminals[executing_terminal].active_pcb->f_desc[fd].flag == 1){
    return -1;
  }
  terminals[executing_terminal].active_pcb->f_desc[fd].flag = 1; // Set the corresponding file descriptor to be available again
  return 0;
}

/*sys_getargs
  Description: Get the argument and store it into the buffer user-level program provides
  INPUT: buf - the buffer to store the arg, nbytes -
  OUTPUT: 0
  RETURN: 0 if success; -1 if failed
  SIDE EFFECT: None
*/
int32_t sys_getargs(uint8_t* buf, int32_t nbytes){
  if(buf == NULL || arg_length == 0 || arg_length > nbytes){
    return -1;
  }
  int i;
  for(i = 0; i < nbytes; i++){
    buf[i] = arg[i];                                /*load the arg buffer*/
  }
  return 0;
}

/*sys_vidmap
  Description: set the user level video address
  INPUT: screen_start: start address for vidmap
  OUTPUT: 0
  RETURN: 0 if success; -1 if failed
  SIDE EFFECT: None
*/
int32_t sys_vidmap(uint8_t** screen_start){
  if(screen_start == (uint8_t **)0){
    return -1;
  }
  if(!(screen_start > (uint8_t **)VIRSTART && screen_start < (uint8_t **)VIREND)){
    return -1;
  }
  *screen_start = (uint8_t*)(FARPAGE + executing_terminal * FOUR_KB + FOUR_KB * TIMES);
  return 0;
}

/*sys_set_handler
  Description: set the system handler
  INPUT: signum: number for signal; handler_address: address for handler
  OUTPUT: -1
  RETURN: -1
  SIDE EFFECT: None
*/
int32_t sys_set_handler(int32_t signum, void* handler_address){
  return -1;
}

/*sys_set_handler
  Description: return the sig
  INPUT: none
  OUTPUT: -1
  RETURN: -1
  SIDE EFFECT: None
*/
int32_t sys_sigreturn(void){
  return -1;
}
