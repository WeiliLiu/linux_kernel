.data
FIRST_ARG = 8
SET_CR0_32_BIT = 0x80000000
SET_CR4_5_BIT = 0x00000010
  /*loadPageDierectory
    Description: Load the address of the page directory into CR3 register
    INPUT: page directory address
    OUTPUT: None
    RETURN: None
    SIDE EFFECT: Load page dierectory into CR3
  */
.text
.globl loadPageDirectory
loadPageDirectory:
push %ebp
mov %esp, %ebp
mov FIRST_ARG(%esp), %eax
mov %eax, %cr3        # set the PDBR
mov %ebp, %esp
pop %ebp
ret


/*enablePaging
  Description: Enable paging function on kernel by setting correct bites on CR0 and CR4
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: Enable Paging function
*/
# load the address onto CR3 register where MMU will find it, then set the 32th bit in the CR0 register
.text
.globl enablePaging
enablePaging:
push %ebp
mov %esp, %ebp
mov %cr4, %eax
or $SET_CR4_5_BIT, %eax
mov %eax, %cr4        # set the cr4
mov %cr0, %eax
or $SET_CR0_32_BIT, %eax
mov %eax, %cr0        # set the cr0
mov %ebp, %esp
pop %ebp
ret

/*flush_TLB
  Description: flush the TLB
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: flush the TLB
*/
.text
.globl flush_TLB
flush_TLB:
push %ebp
mov %esp, %ebp
mov FIRST_ARG(%esp), %eax   # Getting the first argument
mov %cr3, %eax
mov %eax, %cr3      # flushing
mov %ebp, %esp
pop %ebp
ret
