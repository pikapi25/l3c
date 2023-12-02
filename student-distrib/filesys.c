#include "filesys.h"
#include "lib.h"

/*variables for the file system*/
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
void filesys_init(uint32_t* fs_addr)
{
    boot_block=(boot_block_t*)fs_addr;
    inode_head=(inode_t*)(boot_block+1);
    data_block_head=(data_block_t*)(inode_head + boot_block->inode_count);
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
    /*check if index is valid*/
    if (index >= boot_block->dir_count){
        return -1;
    }

    /*the dentry to be copied from*/
    dentry_t* from_dentry = &(boot_block->direntries[index]);

    /*fill in the dentry block*/
    strcpy((int8_t*)dentry->filename, (int8_t*)from_dentry->filename);
    // strcpy((int8_t*)dentry->filename, (int8_t*)from_dentry->filename);
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

    if(strlen((int8_t*)fname)>FILENAME_LEN){
        return -1;
    }
    
    int i;
    dentry_t* curr_dentry;
    /*scan through the directory entries in the boot block*/
    for(i = 0; i < boot_block->dir_count; i++){
        curr_dentry = &(boot_block->direntries[i]);
        /*call read_dentry_by_index if the file name is found*/
        if(strncmp((int8_t*)fname, (int8_t*)curr_dentry->filename, FILENAME_LEN)==0){
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



// open_file
// find the directory entry corresponding to the named file, allocate an unused file descriptor,
// and set up any data necessary to handle the given type of file (directory,
// RTC device, or regular file). If the named file does not exist or no descriptors are free, the call returns -1.
// input: filename
// output: return 0 on success and -1 on failure
// side effect: none
int32_t open_file(const uint8_t* filename){
    // here we call read_dentry_by_name to test whether the filename is valid and set up any data necessary
    dentry_t d;
    if (read_dentry_by_name(filename,&d) == -1){return -1;}
    return 0;
}

// close_file
// close a file by closing its fd
// input: fd (file descriptor)
// output: return 0 on success and -1 on failure (trying to close default fd == 0 or 1)
// side effect: close a fd
int32_t close_file(int32_t fd){
    if (fd == 0 || fd == 1)return -1;
    return 0;
}

// **read file
// read nbytes data from the buffer and update file_pos once succeed
// input: fd --- file descriptor
//        buf --- buffer offered to read data from
//        nbytes --- read data byte limit
// output: return  on success
// side effect: update file_pos after read is succeeded
int32_t read_file(int32_t fd, void* buf, int32_t nbytes){
    pcb_t* cur_pcb = get_cur_pcb();
    uint32_t inode = cur_pcb->fd_arr[fd].inode;
    uint32_t offset = cur_pcb->fd_arr[fd].file_position;
    int32_t result = read_data(inode, offset, buf, nbytes);
    // since read_data return the bytes copied on success
    if (result != -1){
        // we update the file_pos if read call is succeeded
        cur_pcb->fd_arr[fd].file_position += result;
        return result;
    }
    return 0;
}

// write file
// Writes to regular files should always return -1 to indicate failure since the file system is read-only
// input: fd
//        buf
//        nbytes
// output: -1
// side effect: none
int32_t write_file(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

// open_dir
// open a directory if the dirname is vaild
// input: filename -- name of directory going to be opened
// output: return 0 on success and -1 on failure
// side effect: none
int32_t open_dir(const uint8_t* filename){
    return 0;
}

// close_dir
// input: fd
// output: 0 on success and -1 on failure
// side effect: fd is closed
int32_t close_dir(int32_t fd){
    if (fd == 0 || fd == -1)return -1;
    return 0;
}

// read_dir
// provide current filename
// if the initial location is at or beyond the end of the file then return 0
// input:   fd
//          buf
//          nbytes
// output: number of bytes read
// side effect
int32_t read_dir(int32_t fd, void* buf, int32_t nbytes){
    dentry_t dentry;
    uint32_t length;
    pcb_t* cur_pcb = get_cur_pcb();
    int32_t position = cur_pcb->fd_arr[fd].file_position;
    if (read_dentry_by_index(position, &dentry) == -1)return 0;
    strcpy_filename(buf,(int8_t*)&dentry.filename);
    cur_pcb->fd_arr[fd].file_position = position+1;
    length = strlen((int8_t*)&dentry.filename);
    if (length > FILENAME_LEN){
        return FILENAME_LEN;
    }
    return length;
}



// write_dir
// Writes to regular files should always return -1 to indicate failure since the file system is read-only
// input: fd
//        buf
//        nbytes
// output: -1
// input:
// output:
// side effect
int32_t write_dir(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}
