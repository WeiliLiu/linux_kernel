#ifndef FILE_H
#define FILE_H

#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "paging.h"
#include "types.h"
#include "pcb.h"


#define FILENAME_LEN 32
#define ENTRIES_NUM 63
#define BLOCK_SIZE 4096   //4KB
#define data_BNUM 1023
#define RESER_DEN 24
#define RESER_BOOT 52
#define UPPB 7
#define LOWB 0
#define FIRST_REG_FILE 2
#define LAST_REG_FILE 8
#define DIR_NAME_LENGTH 32

// dentry struct
typedef struct dentry{
  int8_t filename[FILENAME_LEN];
  int32_t filetype;
  int32_t inode_num;
  int8_t reserved[RESER_DEN];
}dentry_t;

// inode struct
typedef struct inode{
  int32_t length;
  int32_t data_block_num[data_BNUM];
}inode_t;

//boot_block struct
typedef struct boot_block{
  int32_t dir_count;
  int32_t inode_count;
  int32_t data_count;
  int8_t reserved[RESER_BOOT];
  dentry_t direntries[ENTRIES_NUM];
}boot_block_t;

//data_block struct
typedef struct data_block{
  int8_t data[BLOCK_SIZE];
}data_block_t;

// global variables
extern boot_block_t* boot_block_ptr;
extern dentry_t* dentry_ptr;
extern pcb_t* pcb_ptr;

//file init
extern void file_init(const uint32_t file_start_addr);
//search in all the directory and check if the file exists. if do, then copy all the info into dentry
extern int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
//search in all the directory and check if the index exists. if do, then copy all the info into dentry
extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
//find the index file and start at the offset, read the file for certain length and then copy all the data read into a buffer
extern int32_t read_data(uint32_t inode,uint32_t offset,uint8_t* buf, uint32_t length );
//find the index file and start at the offset, read the file for certain length and then copy all the data read into a buffer
extern int32_t file_read(int32_t fd, void* buf,int32_t nbytes);
//Read_only system return -1
extern int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
//check if we can open the file we want
extern int32_t file_open(const uint8_t* filename);
//close file
extern int32_t file_close(int32_t fd);
//check if the Inputs are valid, copy the dir_name into the buffer
extern int32_t dir_read(int32_t fd, void* buf,int32_t nbytes);
//Read_only system return -1
extern int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
//check if we can open the directory we want
extern int32_t dir_open(const uint8_t* filename);
//close dir
extern int32_t dir_close(int32_t fd);

#endif
