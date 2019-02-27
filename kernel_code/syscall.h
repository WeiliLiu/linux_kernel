#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "lib.h"
#include "file.h"
#include "pcb.h"
#include "rtc.h"
#include "terminal.h"
#include "x86_desc.h"
#include "paging.h"
#include "paging_function.h"
#include "syscall_wrapper.h"

#define RTC_FILE 0
#define DIR_FILE 1
#define REG_FILE 2
#define EIGHT_KB 0x2000
#define VIRTUALADDR 0x08048000
#define VIRSTART 0x8000000
#define FIRST_WORD 0
#define SECOND_WORD 1
#define THIRD_WORD 2
#define FOURTH_WORD 3
#define MAX_NUM_PROCESS 6
#define LAST_REG_FILE 8
#define VIREND 0x8400000
#define TIMES 4

extern uint32_t num_process_executing;
extern uint32_t processes[MAX_NUM_PROCESS];

extern int32_t sys_halt(uint8_t status);// halt function for system call
extern int32_t sys_execute(const uint8_t* command);// execute function for system call
extern int32_t sys_read(int32_t fd, void* buf, int32_t nbytes);//read function for system call
extern int32_t sys_write(int32_t fd, const void* buf, int32_t nbytes);//write function for system call
extern int32_t sys_open(const uint8_t* fname);//open function for system call
extern int32_t sys_close(int32_t fd);//close function for system call
extern int32_t sys_getargs(uint8_t* buf, int32_t nbytes);//get function arguments
extern int32_t sys_vidmap(uint8_t** screen_start);//set vidmap
extern int32_t sys_set_handler(int32_t signum, void* handler_address);//set sys handler
extern int32_t sys_sigreturn(void);//return sig

// Helper functions to get the fname
void parse_command(const uint8_t* command, uint8_t* fname_buf, uint8_t* arg_buf);
// Helper function to check if a given file is an executable file
int32_t executable_check(uint8_t* buf, int32_t directory_idx);
// Helper functiont open io functions for a given process
void open_io();
//user level load
uint32_t loader(uint8_t* buf);

#endif
