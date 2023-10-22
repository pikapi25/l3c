#include "filesys.h"
#include "lib.h"

/*variables for the file system*/
boot_block_t* boot_block;
inode_t* inode_head;
data_block_t* data_block_head;

// here we record the pointer for available pcb entry -1 using global pointer pcb_pointer
fdt pcb[8];
int32_t pcb_pointer = 1;
int dir_loc = 0;

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


// int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
//     /* Gets the address of the inode to read from */
//     inode_t* curr_inode = inode_head + inode;

//     /* Gets the file size of the corresponding inode to read from */
//     unsigned int file_size = curr_inode->length;

//     /* Gets an array of the data blocks for the corresponding inode to read from */
//     unsigned int* data_blocks = curr_inode->data_block_num;

//     /* Gets the total number of inodes */
//     unsigned int num_inodes = boot_block->inode_count;
    
//     /* Declare other local variables */
//     unsigned int num_bytes_read_total = 0;
//     unsigned int curr_data_block_num = 0;
//     unsigned int curr_byte_index = 0;
//     unsigned int curr_data_block_index;
//     data_block_t* curr_data_block;
//     unsigned int i;

//     /* Checks if the given inode index number is out of bounds */
//     if (inode < 0 || inode >= num_inodes) {
//         return 0;
//     }

//     /* Checks if the given offset value is out of bounds */
//     if (offset < 0 || offset >= file_size ) {
//         return 0;
//     }

//     /* Edge case: whenever the offset is really big it  */
//     /* breaks for some weird reason (specifically in    */
//     /* the very large text file) so we'll just treat    */
//     /* that as an edge case and add the offset.         */
//     if( offset > 4096 && ( offset + 157 ) >= file_size )
//     {
//         return 0;
//     }

//     /* If the number of bytes to read is greater than the file size, */
//     /* set the number of bytes to read to just be equal to the file size */
//     if (length > file_size) {
//         length = file_size;
//     }

//     /* If the offset + number of bytes to read is greater than the file size, */
//     /* set the number of bytes to read to be the difference between the file size and offset value */
//     if (offset + length > file_size) {
//         length = file_size - offset;
//     }  

//     /* Set the current byte to start reading from to be equal to the offset value */
//     curr_byte_index = offset;

//     /* If the current byte to read from is greater than the size of a data block, reset it back to 0 */
//     /* Do this both before AND after reading the data in case our offset passed in is too large.     */
//     if (curr_byte_index >= 4096) {
//         curr_byte_index = 0;

//         /* Update the current data block we are reading from */
//         curr_data_block_num++;
//     }

//     /* Loop through all bytes to read and copy them to the passed in buffer */
//     for (i = 0; i < length; i++) {        
//         /* Gets the corresponding data block to read from */
//         curr_data_block_index = data_blocks[curr_data_block_num];
//         curr_data_block = (data_block_t*) data_block_head + curr_data_block_index;

//         /* Copies over a byte of data from the corresponding data block to the buffer */
//         buf[i] = curr_data_block->data[curr_byte_index];
//         curr_byte_index++;

//         /* If the current byte to read from is greater than the size of a data block, reset it back to 0 */
//         if (curr_byte_index >= 4096) {
//             curr_byte_index = 0;

//             /* Update the current data block we are reading from */
//             curr_data_block_num++;
//         }

//         /* Update the total number of bytes read */
//         num_bytes_read_total++;
//     }

//     return num_bytes_read_total;
// }

// int32_t read_data(uint32_t inode_index, uint32_t offset, uint8_t* buf, uint32_t length){
//     inode_t* inode;
//     uint32_t start_block_index;  // Which data block we should start to read
//     uint32_t start_pos_inblock;  // Which position in the start block we should start to read
//     uint32_t end_block_index;  // Which data block we should end reading
//     uint32_t end_pos_inblock;  // Which position in the last block we should end reading
//     uint32_t i, bytes_copied;
//     uint32_t    cur_datablock_index; // The index of current data block in data_blocks array
//     data_block_t* cur_datablock;   // Pointer to current data block
//     /* Sanity check */
//     /* If inode_index is out of range, we can not read any bytes */
//     if(inode_index > (boot_block->inode_count-1) )
//         return 0;
//     /* If offset is already bigger than the file's total length, we can not read any bytes */
//     if(inode_head[inode_index].length < offset)
//         return 0;

//     length = (length + offset > inode_head[inode_index].length) ?
//                 inode_head[inode_index].length - offset : length;
//     if(length == 0)
//         return 0;

//     inode = &(inode_head[inode_index]);
//     bytes_copied = 0;

//     /* Calculate how many data blocks are used and start/end positions */
//     start_block_index = offset / BLOCK_SIZE;
//     start_pos_inblock = offset % BLOCK_SIZE;
//     end_block_index = (length-1) / BLOCK_SIZE  + start_block_index;
//     end_pos_inblock = (length-1) % BLOCK_SIZE + start_pos_inblock ;

//     for(i=start_block_index; i<=end_block_index; i++){
//         cur_datablock_index = (inode_head[inode_index].data_block_num)[i];
//         cur_datablock = &(data_block_head[cur_datablock_index]);
//         /* Copy the start data block */
//         if(i == start_block_index){
//             if(start_block_index==end_block_index){
//                 /* If start block is the same as end block, only copy part of the data block */
//                 memcpy(buf, &(cur_datablock->data[start_pos_inblock]), length);
//                 buf += length;
//                 bytes_copied += length;
//             }else{
//                 /* else, copy all the data in start block */
//                 memcpy(buf, &(cur_datablock->data[start_pos_inblock]), BLOCK_SIZE-start_pos_inblock); // Copy all the data in block from start pos
//                 buf += BLOCK_SIZE-start_pos_inblock;
//                 bytes_copied += BLOCK_SIZE-start_pos_inblock;
//             }
//             continue;
//         }

//         /* Copy the end data block */
//         if(i==end_block_index){
//             memcpy(buf, cur_datablock->data, end_pos_inblock+1 ); // Copy end_pos + 1 bytes
//             buf += end_pos_inblock+1;
//             bytes_copied += end_pos_inblock+1;
//             break;
//         }

//         /* Copy the data blocks in the middle */
//         memcpy(buf, cur_datablock->data, BLOCK_SIZE); // 4096 bytes in this data block should be copied
//         buf += BLOCK_SIZE;
//         bytes_copied += BLOCK_SIZE;
//     }

//     return bytes_copied;
// }



// open_file
// find the directory entry corresponding to the named file, allocate an unused file descriptor,
// and set up any data necessary to handle the given type of file (directory,
// RTC device, or regular file). If the named file does not exist or no descriptors are free, the call returns -1.
// input: filename
// output: return 0 on success and -1 on failure
// side effect: none
int32_t open_file(const uint8_t* filename){
    // here we call read_dentry_by_name to test whether the filename is valid and set up any data necessary
    dentry_t* d;
    if (read_dentry_by_name(filename,d) == 0){return 0;}
    return -1;
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
    // we use cur_fdt to store current file descriptor entry
    fdt cur_fdt = pcb[fd];
    uint32_t inode = cur_fdt.inode;
    uint32_t offset = cur_fdt.file_pos;
    int32_t result = read_data(inode, offset, buf, nbytes);
    // since read_data return the bytes copied on success
    if (result != -1){
        // we update the file_pos if read call is succeeded
        pcb[fd].file_pos += result; 
        return 0;
    }
    return -1;
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

// // **read_dir
// // provide current filename
// // if the initial location is at or beyond the end of the file then return 0
// // input:   fd
// //          buf
// //          nbytes
// // output: number of bytes read
// // side effect
// int32_t read_dir(int32_t fd, void* buf, int32_t nbytes){
// }

// **read_dir
// provide filename with index #? 
// if the initial location is at or beyond the end of the file then return 0
// input:   fd
//          buf
//          nbytes
// output: number of bytes read
// side effect
int32_t read_dir_index(int32_t fd, void* buf, int32_t index){
    // if beyond the file number range
    if (index >= boot_block->dir_count){return -1;}

    dentry_t dentry = boot_block->direntries[index];
    
    memcpy(buf, &dentry.filename, FILES_NUM_MAX);
    return strlen((int8_t*)dentry.filename);
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
