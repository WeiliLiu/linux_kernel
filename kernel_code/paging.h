#ifndef _PAGING_H
#define _PAGING_H

#include "lib.h"

#define KB_SIZE 1024
#define FOUR_KB_SIZE 4096
#define FOUR_KB 0x1000
#define VIDEO_INDEX 0xB8
#define KERNEL_SPEC  0x83
#define FOUR_MB 0x400000
#define EIGHT_MB 0x800000
#define NOT_PRESENT 0x00000002
#define USER 0x4
#define VIRPRO 32
#define WP 3
#define PRESENT 2
#define USERVIDPAGE 33
#define VIDADDR 0xB8000
#define FARPAGE 0x84B8000
#define INDEX_SEG 4
#define FISH_BOUND 7

uint32_t page_directory[KB_SIZE] __attribute__((aligned(FOUR_KB_SIZE)));//the page directory
uint32_t first_page_table[KB_SIZE] __attribute__((aligned(FOUR_KB_SIZE)));//create new page table
uint32_t user_video_page_table[KB_SIZE] __attribute__((aligned(FOUR_KB_SIZE)));//create new page table

void INIT_PAGE();//refer to that of paging.c
void process_map(uint32_t process);//map the user program
extern void switch_active_terminal(int32_t terminal_num);//repaging when switch terminal

#endif
