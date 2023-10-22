#include "filesys.h"
#include "lib.h"

boot_block_t* boot_block;
inode_t* inode_head;
data_block_t* data_block_head;


/*
*   filesys_init
*   Description: initialize some variables for the file system
*   Inputs: uint32_t* fs_start - the address of the beginning of the file system
*   Outputs: none
*   Side effects: none
*/
void filesys_init(uint32_t* fs_start)
{
    boot_block=(boot_block_t*)fs_start;
    inode_head=(inode_t*)(boot_block+1);
    data_block_head=(data_block_t*)(boot_block + boot_block->inode_count);
}

/**
 * read_dentry_by_index
 * 
 * Description: This function retrieves a directory entry by its index from the boot block
 * and copies the information into the provided 'dentry' structure.
 * Inputs:  index - The index of the dentry to read.
 *          dentry - Pointer to the dentry structure where the data will be copied.
 * Return value:  0 on success, -1 on failure.
 * Side effects: none
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
    /*check if index is within the boundries*/
    if (index >= boot_block->dir_count){
        return -1;
    }

    /*the dentry to be copied from*/
    dentry_t* from_dentry = &(boot_block->direntries[index]);

    /*fill in the dentry block*/
    strncpy((int8_t*)dentry->filename, (int8_t*)from_dentry->filename, FILENAME_LEN);
    dentry->filetype = from_dentry->filetype;
    dentry->inode_num = from_dentry->inode_num;

    return 0;
}


/**
 * read_dentry_by_name
 * 
 * Description: This function searches for a directory entry by its filename in the boot block
 * and copies the information into the provided 'dentry' structure.
 * Inputs:  fname - Pointer to the filename to search for.
 *          dentry - Pointer to the dentry structure where the data will be copied.
 * Return value:  0 on success, -1 on failure.
 * Side effects: none
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
    /*check if the file exists*/
    if(fname==NULL){
        return -1;
    }

    /*check if the file name is valid*/
    if(strlen(fname)>FILENAME_LEN){
        return -1;
    }
    
    int i;
    dentry_t* curr_dentry;
    /*scan through the directory entries in the boot block*/
    for(i = 0; i < boot_block->dir_count; i++){
        curr_dentry = &(boot_block->direntries[i]);
        /*call read_dentry_by_index if the file name is found*/
        if(strncmp((int8_t*)fname,(int8_t*)curr_dentry->filename,FILENAME_LEN)==0){
            read_dentry_by_index(i,dentry);
            return 0;
        }
    }

    /*fail to find the file*/
    return -1;
}


/**
 * read_data
 * 
 * Description: This function reads data from a file associated with the specified inode.
 * Inputs:  inode - The inode number of the file to read.
 *          offset - The starting offset within the file to begin reading.
 *          buf - Pointer to the buffer where the data will be copied.
 *          length - The number of bytes to read from the file.
 * Return value:  The number of bytes read on success, -1 on failure.
 * Side effects: none
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
    inode_t* target_file = inode_head + inode;
    /*check if inode number is valid*/
    if (inode >= boot_block->inode_count){
        return -1;
    }

    /*check if offset is within the file*/
    if(offset >= target_file->length){
        return -1;
    }

    /*if reading range exceed the file, read until the end of the file*/
    if(length + offset > target_file->length){
        length = target_file->length - offset;
    }

    int32_t i, num_bytes=0; 
    int32_t curr_block_idx;
    data_block_t* curr_block;
    int32_t curr_block_num = offset / BLOCK_SIZE;       //current block number in the inode
    int32_t curr_offset = offset - curr_block_num * BLOCK_SIZE;     //current offset within the data block
    
    /*read through each byte and copy to the buffer*/
    for(i = 0; i < length; i++){
        curr_block_idx = target_file->data_block_num[curr_block_num];
        curr_block = (data_block_t*)(data_block_head + curr_block_idx);

        /*copy bytes into buffer*/
        buf[i]=curr_block->data[curr_offset];
        curr_offset++;

        /*current block copy has completed, go to next block*/
        if(curr_offset >= BLOCK_SIZE){
            curr_block_num++;
            curr_offset=0;
        }

        /*increment the number of bytes processed*/
        num_bytes++;
    }

    return num_bytes;
}