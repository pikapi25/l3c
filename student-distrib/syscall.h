#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "terminal.h"

/* Magic Numbers*/
#define MAX_PROCESSES 6
#define MAX_FILES 8
#define PCB_BLOCK_SIZE 0x2000 // every pcb block is 8KB
#define PCB_START 0x800000 // every pcb starts at 8MB-i*8KB

/* system call functions */
int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);

/* file operations */
typedef int32_t (*read_file)(int32_t fd, void* buf, int32_t nbytes);
typedef int32_t (*write_file)(int32_t fd, const void* buf, int32_t nbytes);
typedef int32_t (*open_file)(const uint8_t* filename);
typedef int32_t (*close_file)(int32_t fd);

typedef struct file_op
{
    read_file read_op;
    write_file write_op;
    open_file open_op;
    close_file close_op;
}file_op_t;

/* file descriptor */
typedef struct file_desc
{
    file_op_t* file_op_table;
    int32_t inode;
    int32_t file_postion;
    int32_t flags;

}file_desc_t;

/* Process Control Block */
typedef struct pcb
{
    file_desc_t fd_arr[MAX_FILES];
    uint8_t arg_buf[MAX_CHA_BUF];
    uint32_t pid;
    uint32_t parent_pid;
    int32_t ebp_val;
    int32_t esp_val;

}pcb_t;

/* ----- Helper Functions ----- */
uint32_t get_pid();
pcb_t* get_pcb(uint32_t pid);

#endif /* _SYSCALL_H */