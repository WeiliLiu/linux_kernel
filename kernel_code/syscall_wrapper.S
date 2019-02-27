.data
HALT_SYS_NUM = 1
EXEC_SYS_NUM = 2
READ_SYS_NUM = 3
WRITE_SYS_NUM = 4
OPEN_SYS_NUM = 5
CLOSE_SYS_NUM = 6
GETARG_SYS_NUM = 7
VIDMAP_SYS_NUM = 8
SETHAND_SYS_NUM = 9
SIGR_SYS_NUM = 10
SYSCALL_ENTRY_NUM = 0x80
FIRST_ARG = 0x08
SECOND_ARG = 0x0C
THIRD_ARG = 0x10
BYTES_PER_MEMORY = 4

.text
.globl syscall_wrapper
.globl halt
.globl execute
.globl read
.globl write
.globl open
.globl close
.globl getargs
.globl vidmap
.globl set_handler
.globl sigreturn
.align BYTES_PER_MEMORY /* maks sure each memory takes up 4 bytes */

/*halt
  Description: user level wrapper function for halt
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: system call interrupts
*/
halt:
  pushl %ebx
  movl FIRST_ARG(%esp), %ebx
  movl $HALT_SYS_NUM, %eax              /*halt*/
  int $SYSCALL_ENTRY_NUM
  popl %ebx
  ret

/*execute
    Description: user level wrapper function for execute
    INPUT: None
    OUTPUT: None
    RETURN: None
    SIDE EFFECT: system call interrupts
*/
execute:
  pushl %ebx
  movl FIRST_ARG(%esp), %ebx
  movl $EXEC_SYS_NUM, %eax              /*execute*/
  int $SYSCALL_ENTRY_NUM
  popl %ebx
  ret


/*read
    Description: user level wrapper function for read
    INPUT: None
    OUTPUT: None
    RETURN: None
    SIDE EFFECT: system call interrupts
*/
read:
  pushl %ebx
  movl THIRD_ARG(%esp), %edx
  movl SECOND_ARG(%esp), %ecx
  movl FIRST_ARG(%esp), %ebx
  movl $READ_SYS_NUM, %eax                    /*sys read*/
  int $SYSCALL_ENTRY_NUM
  popl %ebx
  ret

/*write
    Description: user level wrapper function for write
    INPUT: None
    OUTPUT: None
    RETURN: None
    SIDE EFFECT: system call interrupts
*/

write:
  pushl %ebx
  movl THIRD_ARG(%esp), %edx
  movl SECOND_ARG(%esp), %ecx
  movl FIRST_ARG(%esp), %ebx
  movl $WRITE_SYS_NUM, %eax                 /*sys write*/
  int $SYSCALL_ENTRY_NUM
  popl %ebx
  ret

/*open
    Description: user level wrapper function for open
    INPUT: None
    OUTPUT: None
    RETURN: None
    SIDE EFFECT: system call interrupts
*/
open:
  pushl %ebx
  movl FIRST_ARG(%esp), %ebx
  movl $OPEN_SYS_NUM, %eax                /*sys open*/
  int $SYSCALL_ENTRY_NUM
  popl %ebx
  ret

/*close
    Description: user level wrapper function for close
    INPUT: None
    OUTPUT: None
    RETURN: None
    SIDE EFFECT: system call interrupts
*/
close:
  pushl %ebx
  movl FIRST_ARG(%esp), %ebx
  movl $CLOSE_SYS_NUM, %eax                /*sys close*/
  int $SYSCALL_ENTRY_NUM
  popl %ebx
  ret

/*getargs
    Description: user level wrapper function for getargs
    INPUT: None
    OUTPUT: None
    RETURN: None
    SIDE EFFECT: system call interrupts
*/
getargs:
  pushl %ebx
  movl SECOND_ARG(%esp), %ecx
  movl FIRST_ARG(%esp), %ebx
  movl $GETARG_SYS_NUM, %eax                 /*sys write*/
  int $SYSCALL_ENTRY_NUM
  popl %ebx
  ret

/*vidmap
    Description: user level wrapper function for vidmap
    INPUT: None
    OUTPUT: None
    RETURN: None
    SIDE EFFECT: system call interrupts
*/
vidmap:
  pushl %ebx
  movl FIRST_ARG(%esp), %ebx
  movl $VIDMAP_SYS_NUM, %eax                 /*sys write*/
  int $SYSCALL_ENTRY_NUM
  popl %ebx
  ret

/*set_handler
      Description: user level wrapper function for sys_set_handler
      INPUT: None
      OUTPUT: None
      RETURN: None
      SIDE EFFECT: system call interrupts
*/
set_handler:
  pushl %ebx
  movl SECOND_ARG(%esp), %ecx
  movl FIRST_ARG(%esp), %ebx
  movl $SETHAND_SYS_NUM, %eax                 /*sys write*/
  int $SYSCALL_ENTRY_NUM
  popl %ebx
  ret

/*sigreturn
      Description: user level wrapper function for sigreturn
      INPUT: None
      OUTPUT: None
      RETURN: None
      SIDE EFFECT: system call interrupts
*/
sigreturn:
  pushl %ebx
  movl $SIGR_SYS_NUM, %eax
  int $SYSCALL_ENTRY_NUM
  popl %ebx
  ret

/*syscall_wrapper
  Description: syscall wrapper function for system call interrupt
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: system call interrupts
*/

syscall_wrapper:
  pushl %es
  pushl %ds
  pushl %ebx
  pushl %ecx
  pushl %edx
  pushl %esi
  pushl %edi
  pushl %ebp
  pushfl
  cli
  cmpl $1, %eax
  jb SYSCALL_NUM_ERROR
  cmpl $SIGR_SYS_NUM, %eax
  ja SYSCALL_NUM_ERROR              /*check sys call validity*/
  subl $1, %eax
  pushl %edx
  pushl %ecx
  pushl %ebx
  sti
  call *syscall_jump_table(, %eax, BYTES_PER_MEMORY)   /*use jumptable to select syscall*/
  popl %ebx
  popl %ecx
  popl %edx
  jmp DONE

SYSCALL_NUM_ERROR:
  movl $-1, %eax

DONE:
  popfl
  popl %ebp
  popl %edi
  popl %esi
  popl %edx
  popl %ecx
  popl %ebx
  popl %ds
  popl %es              /*pop registers*/
  iret


syscall_jump_table:
  .long sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn
