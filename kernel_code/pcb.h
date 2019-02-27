#ifndef _PCB_H
#define _PCB_H

#define NUM_FUNC 4
#define NUM_FD 8

// struct of file operations
typedef struct file_operations{
  int32_t (*open) (const uint8_t* filename);
  int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
  int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
  int32_t (*close) (int32_t fd);
}file_operations_t;

// struct of file_entry
typedef struct file_entry{
  file_operations_t ft_ptr;
  int32_t inode;
  int32_t file_position;
  int32_t flag;
}f_desc_t;

// struct of pcb
typedef struct pcb{
  f_desc_t f_desc[NUM_FD];
  uint32_t process_num;
  uint32_t* parent_pcb;
  uint32_t curr_esp0;
  uint32_t ebp;
  uint32_t esp;
}pcb_t;



#endif
