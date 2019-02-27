#ifndef _PAGING_FUNCTION_H
#define _PAGING_FUNCTION_H

extern void loadPageDirectory(unsigned int*); //refer to that in the paging_function.S
extern void enablePaging(); // Enabing paging function on kernel
extern void flush_TLB();//flush the tlb in .s file

#endif
