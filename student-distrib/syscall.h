#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "terminal.h"

/* Magic Numbers*/
#define MAX_PROCESSES 6
#define MAX_FILES 8
#define PCB_BLOCK_SIZE 0x2000 // every pcb block is 8KB
#define PCB_START 0x800000 // every pcb starts at 8MB-i*8KB
#define PCB_BITMASK 0xFFFFE000 // the mast to get current pcb from esp

/* system call functions */
int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);

/* file operations */
typedef int32_t (*read_f)(int32_t fd, void* buf, int32_t nbytes);
typedef int32_t (*write_f)(int32_t fd, const void* buf, int32_t nbytes);
typedef int32_t (*open_f)(const uint8_t* filename);
typedef int32_t (*close_f)(int32_t fd);

typedef struct file_op
{
    read_f read_op;
    write_f write_op;
    open_f open_op;
    close_f close_op;
}file_op_t;

/* file descriptor */
typedef struct file_desc
{
    file_op_t* file_op_table;
    int32_t inode;
    int32_t file_position;
    int32_t flags;

}file_desc_t;

/* Process Control Block */
typedef struct pcb pcb_t;
struct pcb
{
    file_desc_t fd_arr[MAX_FILES];
    uint8_t arg_buf[MAX_CHA_BUF];
    uint32_t pid;
    pcb_t* parent_pcb;
    int32_t ebp_val;
    int32_t esp_val;

};

/* ----- Helper Functions ----- */
pcb_t* get_cur_pcb();

#endif /* _SYSCALL_H */