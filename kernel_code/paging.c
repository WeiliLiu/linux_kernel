#include "paging.h"
#include "types.h"
#include "paging_function.h"

/*INIT_PAGE
  Description: Initialize page
  INPUT: None
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: None
*/

void INIT_PAGE(){
  //set each entry to not present
  int i;
  for(i = 0; i < KB_SIZE; i++)
  {
    // This sets the following flags to the pages:
    //   Supervisor: Only kernel-mode can access them
    //   Write Enabled: It can be both read from and written to
    //   Not Present: The page table is not present
    page_directory[i] = NOT_PRESENT;  /*not present the PDEs*/
    first_page_table[i] = (i * FOUR_KB) | PRESENT;  // attributes: supervisor level, read/write, present.
  }

  first_page_table[VIDEO_INDEX] =  first_page_table[VIDEO_INDEX] | WP; /*present the video memory*/

  page_directory[0] = ((unsigned int)first_page_table) | WP;   /*present the PDE*/
  page_directory[1] = FOUR_MB | KERNEL_SPEC;                /*SET THE ONE TO ONE kernel memory*/


  for(i = 0; i < KB_SIZE; i++)
  {
    user_video_page_table[i] = (i * FOUR_KB) | PRESENT;  // initialize all pages in the page table to be not present
  }
  user_video_page_table[VIDEO_INDEX] = VIDADDR | WP | USER;
  /* set up vid memory buffer for screen switch */
	for(i = 1; i < FISH_BOUND; i++){
		user_video_page_table[VIDEO_INDEX + i] = (VIDADDR + i * FOUR_KB) | WP | USER;
	}

  page_directory[USERVIDPAGE] = ((unsigned int)user_video_page_table) | WP | USER;


  loadPageDirectory(page_directory);    /*load PDE*/
  enablePaging();       /*SET THE control registers*/
}

/*process_map
  Description: map the page for the process
  INPUT: process - process number
  OUTPUT: None
  RETURN: None
  SIDE EFFECT: Change the process's paging
*/
void process_map(uint32_t process){
  uint32_t process_addr = EIGHT_MB + FOUR_MB * process;                       /*get the physical address*/
  page_directory[VIRPRO] = process_addr | KERNEL_SPEC | USER;                     /*map it to 128MB in virtual*/
  flush_TLB();
}



/*switch_active_terminal
  Description: remap the video paging when switching between terminal
  INPUT: terminal_num
  OUTPUT: none
  SIDE EFFECT: remap some new pages
*/
void switch_active_terminal(int32_t terminal_num){
  memcpy((void*)(FARPAGE+(displayed_terminal+1)*FOUR_KB), (void*)VIDADDR, (uint32_t)FOUR_KB_SIZE);
	clear();
	memcpy((void*)(VIDADDR), (void*)(FARPAGE+(terminal_num+1)*FOUR_KB), (uint32_t)FOUR_KB);

  user_video_page_table[VIDEO_INDEX + displayed_terminal + INDEX_SEG] = (VIDADDR + (INDEX_SEG + displayed_terminal) * FOUR_KB) | WP | USER;
	/* map the right buffer to the VEDIO MEM */
	user_video_page_table[VIDEO_INDEX + terminal_num + INDEX_SEG] = VIDADDR | WP | USER;

  displayed_terminal = terminal_num;
  flush_TLB();
}
