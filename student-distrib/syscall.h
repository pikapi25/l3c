#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "terminal.h"
#include "filesys.h"
#include "rtc.h"

/* Magic Numbers*/
#define MAX_PROCESSES 6
#define MAX_FILES 8
#define NUM_SCHE  3
#define PCB_BLOCK_SIZE 0x2000 // every pcb block is 8KB
#define PCB_BOTTOM 0x800000 // every pcb starts at 8MB-i*8KB
#define PCB_BITMASK 0xFFFFE000 // the mast to get current pcb from esp

#define VIRTUAL_PAGE_START 0x08000000 // All user level programs will be loaded in the page starting at 128MB (virtual mem)
#define PHYS_PROGRAM_START 0x800000  
#define PHYS_PROGRAM_SIZE 0x400000 // every program is 4MB in physical mem
#define USER_CODE   0x8048000  // starting point of user code
#define USER_STACK  0x8400000  // 132 MB
#define Free 0
#define Busy 1
/* system call functions */
         
#define FAILURE         -1              /* Used to return -1 when a function has failed */
#define MAGIC_NUM_0     0x7F            /* Magic numbers that identify executables      */
#define MAGIC_NUM_1     0x45            
#define MAGIC_NUM_2     0x4C
#define MAGIC_NUM_3     0x46
#define ADDR_BIT        24
#define EIGHT_MB        0x00800000      
#define EIGHT_KB        0x2000     
#define USER_PROGRAM_IMG_START   0x8000000   //128MB
#define USER_PROGRAM_END     0x8800000  //128MB+8MB

int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);


/* file operations */
typedef int32_t (*read_f)(int32_t fd, void* buf, int32_t nbytes);
typedef int32_t (*write_f)(int32_t fd, const void* buf, int32_t nbytes);
typedef int32_t (*open_f)(const uint8_t* filename);
typedef int32_t (*close_f)(int32_t fd);

typedef struct file_op
{
    open_f open_op;
    close_f close_op;
    read_f read_op;
    write_f write_op;
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
    uint8_t arg_buf[MAX_CHA_BUF+1];
    uint32_t pid;
    pcb_t* parent_pcb;
    int32_t ebp_val;
    int32_t esp_val;
    int32_t sch_ebp;
};

/* ----- Helper Functions ----- */
pcb_t* get_cur_pcb();

#endif /* _SYSCALL_H */
