#ifndef _FILESYS_H
#define _FILESYS_H

#include "type.h"

#define FILENAME_LEN 32;        //name is up to 32 characters
#define FILES_NUM_MAX 63;       //the file system can hold up to 63 files
#define DATA_BLOCK_COUNT_MAX 1023;        //(4096-4)/4=1023   4096->4kB per block
#define BLOCK_SIZE 4096;

//data structures for the file system
typedef struct dentry
{
    int8_t filename[FILENAME_LEN];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserved[24];        //24B reserved
} dentry_t;

typedef struct boot_block
{
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[52];        //52B reserved
    dentry_t direntries[FILES_NUM_MAX]; 
} boot_block_t;

typedef struct inode
{
    int32_t length;
    int32_t data_block_num[DATA_BLOCK_COUNT_MAX];
} inode_t;

typedef struct data_block
{
    int8_t data[BLOCK_SIZE];
} data_block_t;


extern void filesys_init(uint32_t* fs_start);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif
