#include "file.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "paging.h"
#include "types.h"
#include "syscall.h"

boot_block_t* boot_block_ptr;
dentry_t* dentry_ptr;

/*
* file_init;
*   Inputs: starting addr of the boot block
*   Outputs: nothing
*	Description: initialize the current directory, starting boot block, and a file array
* SideEffect: None
*/
void file_init(const uint32_t file_start_addr){
  boot_block_ptr = (boot_block_t*)file_start_addr;                                /*set boot block address*/
  dentry_ptr = (dentry_t*)((uint32_t)boot_block_ptr + (ENTRIES_NUM + 1) + 1);     /*set initial directory entry address in boot block*/
}


/*
* dentry_copy;
*   Inputs: offset in the file to start copy
*           the pointer of dentry
*   Outputs: nothing
*	Description: copy the dentry with a offset
* SideEffect: None
*/
void dentry_copy(uint32_t offset, dentry_t* dentry){
  strncpy(dentry->filename,(dentry_ptr + offset)->filename,FILENAME_LEN);         // copy the filename
  dentry->filetype = (dentry_ptr + offset)->filetype;                             // copy the filetype
  dentry->inode_num = (dentry_ptr + offset)->inode_num;                           // copy the inode num
}

/*
* read_dentry_by_name;
*   Inputs: pointer to intended file name, dentry to read out
*   Outputs: -1 if failed, 0 success
*	Description: search in all the directory and check if the file exists. if do, then copy all the info into dentry
* SideEffect:None
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
  // check for valid inputs
  if(fname == NULL || dentry == NULL){
    return -1;
  }
  int i;
  //search in all the directory and check if the file exists. if do, then copy all the info into dentry
  for(i=0;i<ENTRIES_NUM;i++){
    if(strncmp((int8_t*)(dentry_ptr + i)->filename, (int8_t*)fname,FILENAME_LEN)==0){
      int32_t ret = read_dentry_by_index(i, dentry);
      if(ret == -1){
        return -1;
      }
      break;
    }
  }
  return 0;
}


/*
* read_dentry_by_index;
*   Inputs: pointer to intended index, dentry to read out
*   Outputs: -1 if failed, 0 success
*	Description: search in all the directory and check if the index exists. if do, then copy all the info into dentry
* SideEffect: None
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
  if(((dentry_ptr+index)->filename == NULL) || dentry == NULL){
    return -1;
  }
  dentry_copy(index, dentry);
  return 0;
}


/*
* read_data;
*   Inputs: inode: the index node which we will read data out of it
			offset:starting in the file at this offset
			buf: the output buffer
			length: the length of data we need to read
*   Outputs: -1 if failed, otherwise success
*	Description: find the index file and start at the offset, read the file for certain length and then copy all the data read into a buffer
* SideEffect: NONE
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
  // inode number falls outside of the valid range
  if(inode > boot_block_ptr->inode_count)
    return -1;

  uint32_t start = offset;
  inode_t* cur_inode= (inode_t*)((uint32_t)boot_block_ptr + (inode + 1) * BLOCK_SIZE);                            /*set file pointer*/
  data_block_t* data_start = (data_block_t*)((uint32_t)boot_block_ptr + (((ENTRIES_NUM + 1)) + 1) * BLOCK_SIZE);  /*data block starting address*/

  if((inode > ENTRIES_NUM) || (length == 0) || (buf == NULL) || (offset > cur_inode -> length)||
  (start / BLOCK_SIZE > boot_block_ptr->data_count))                                                                /*invalid case*/
    return -1;

  int i;
  for(i = 0; i < length && start < cur_inode->length; i++){
    buf[i] = ((data_block_t*)((uint32_t)data_start
    + cur_inode->data_block_num[start / BLOCK_SIZE] * BLOCK_SIZE))->data[start % BLOCK_SIZE];  /*copy data from data block to buf*/
    start++;
  }

  return i;
}

/*
* file_read;
*   Inputs: fd: the file descriptor number
			buf: the output buffer
			nbytes: the length of data we need to read
*   Outputs: -1 failed, otherwise success
*	Description: find the index file and start at the offset, read the file for certain length and then copy all the data read into a buffer
* SideEffect: None
*/
int32_t file_read(int32_t fd, void* buf, int32_t nbytes){
  if(fd < LOWB || fd > UPPB){
    return -1;
  }
  inode_t* cur_inode= (inode_t*)((uint32_t)boot_block_ptr +
  (terminals[executing_terminal].active_pcb->f_desc[fd].inode + 1) * BLOCK_SIZE);
  // if the postion is out of range, set it to 0
  if(terminals[executing_terminal].active_pcb->f_desc[fd].file_position >= cur_inode->length){
    return 0;
  }
  //get the postion where the file is read right now
  int32_t file_ff = read_data(terminals[executing_terminal].active_pcb->f_desc[fd].inode,
     terminals[executing_terminal].active_pcb->f_desc[fd].file_position, (uint8_t*)buf, nbytes);
  // check if the postion is valid
  if(file_ff != -1){
    //update the file postion
    terminals[executing_terminal].active_pcb->f_desc[fd].file_position += file_ff;
  }
  return file_ff;
}

/*
* file_write;
*   Inputs: fd: the file descriptor number
			buf: the output buffer
			nbytes: the length of data we need to write
*   Outputs: -1 failed, 0 success
*	Description: Read_only system return -1
* SideEffect: None
*/
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){
  return -1;
}

/*
* file_open;
*   Inputs: the pointer of the filename we want to open
*   Outputs: -1 failed, otherwise success
*	Description: check if we can open the file we want
* SideEffect: None
*/
int32_t file_open(const uint8_t* filename){
  dentry_t* temp_dentry;
  int32_t ret = read_dentry_by_name(filename, temp_dentry);
  if(ret == -1){
    return -1;
  }else {
    return 0;
  }
}

/*
* file_close;
*   Inputs: file descriptor number
*   Outputs: 0 if success
*	Description: nothing
* SideEffect: None
*/
int32_t file_close(int32_t fd){
  return 0;
}
/***********************************************directory functions******************************************/


/*
* dir_read;
*   Inputs: fd: the file descriptor number
			buf: the output buffer
			nbytes: the length of data we need to read
*   Outputs: -1 failed, otherwise success
*	Description: check if the Inputs are valid, copy the dir_name into the buffer
* SideEffect: None
*/
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes){
  uint8_t* buffer = buf;
  // check input
  if(buf == NULL){
    return -1;
  }else if(fd < LOWB || fd > UPPB){
    return -1;
  }

  int i;

  int8_t buflength = 0;
  //get the length of directory name
  uint32_t name_length =
  strlen((const int8_t*)(&(boot_block_ptr->direntries[terminals[executing_terminal].active_pcb->f_desc[fd].file_position].filename)));
  for(i = 0; i < name_length && i < nbytes && i < DIR_NAME_LENGTH; i++){
    //fill the buffer with directory filename
    buffer[i] = boot_block_ptr->direntries[terminals[executing_terminal].active_pcb->f_desc[fd].file_position].filename[i];
    buflength ++;
  }
  terminals[executing_terminal].active_pcb->f_desc[fd].file_position++;
  return buflength;
}

/*
* dir_write;
*   Inputs: fd: the file descriptor number
			buf: the output buffer
			nbytes: the length of data we need to write
*   Return Value: -1 failed, 0 success
*	Description: Read_only system return -1
* SideEffect: None
*/
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes){
  return -1;
}

/*
* dir_open;
*   Inputs: the pointer of the filename we want to open
*   Outputs: -1 failed, otherwise success
*	Description: check if we can open the directory we want
* SideEffect: None
*/
int32_t dir_open(const uint8_t* filename){
  dentry_t* temp_dentry;
  int32_t ret = read_dentry_by_name(filename, temp_dentry);
  if(ret == -1){
    return -1;
  }else {
    return 0;
  }
}

/*
* dir_close;
*   Inputs: file descriptor number
*   Outputs: 0 if success
*	Description: nothing
* SideEffect: None
*/
int32_t dir_close(int32_t fd){
  return 0;
}
