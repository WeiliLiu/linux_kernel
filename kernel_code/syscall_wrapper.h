#ifndef _SYSCALL_WRAPPER_H
#define _SYSCALL_WRAPPER_H

#include "syscall.h"

extern int32_t halt(uint8_t status);//sys halt
extern int32_t execute(const uint8_t* command);//sys execute
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);//sys read
extern int32_t write(int32_t fd, const void* buf, int32_t nbytes);//sys write
extern int32_t open(const uint8_t* filename);//sys open
extern int32_t close(int32_t fd);//sys close
extern int32_t getargs(uint8_t* buf, int32_t nbytes); //system call function getargs
extern int32_t vidmap(uint8_t** screen_start);
extern int32_t set_handler(int32_t signum, void* handler_address);
extern int32_t sigreturn(void);
extern void syscall_wrapper();//sys wrapper

#endif
