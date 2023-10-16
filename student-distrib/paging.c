#include "paging.h"
#include "types.h"

// page table and page dictionary all contain 1024 entries
// and each entry is 32 bit, so use uint32_t
#define PAGE_TABLE_COUNT    1024
#define PAGE_DIC_COUNT  1024
#define KERNEL_PHYSICAL_ADDRESS 0x400000
// PT and PD are required to be aligned.
// since there are 1024 entries for each array and each entry is 4 Byte long
// we use 4*1024 to make every entry aligned by 4 bytes

// PDE has following structure
// |31 ... 12|11 ... 8| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
// | Address |   AVL  | PS| D | A |PCD|PWT|U/S|R/W| P |
// (0) P : present. set 1 when the page is present. set 0 indicates being ignored.
// (1) R/W : set 1 when page is writable. set 0 when page is read only.
// (2) U/S : set 1 when user can visit. set 0 when user is forbidden and only supervisor(kernel) is allowed.
// (3) PWT : set 1 when using write through. set 0 when using write back.
// (4) PCD : set 1 when disable page cache. set 0 when enable page cache.
// (5) A : set 1 when the page is being accessed once. set 0 by the system.
// (6) D : set 1 when the page is written before. set 0 if the page is not changed.
// (7) PS : set 1 when PDE is mapped into 4MB page. set 0 when PDE is mapped into 4 KB page table.
// (11 ... 8) AVL : is ignored.
// (31 ... 12) Address : For PDE, it's the address of PT
//                       For PTE, it's physical address of 4kb memory space (memory frame)
//  PDE and PTE has small difference in PAT (bit 7)
uint32_t page_table[PAGE_TABLE_COUNT] __attribute__((aligned (4 * PAGE_TABLE_COUNT)));        // page_table for 0~4 MB
uint32_t page_table_extra[PAGE_TABLE_COUNT] __attribute__((aligned (4 * PAGE_TABLE_COUNT)));        // page_table for extra use
uint32_t page_dic[PAGE_DIC_COUNT] __attribute__((aligned (4 * PAGE_DIC_COUNT)));            


// Page_Initialize
// We open Paging mode, and fill some entries into PD and PT
//      There is only two entries needed
//      1. kernel (4MB page)
//      2. video map
// input: no
// output : no
// side effect: change PD,PT and cr0, cr3, cr4
void 
Page_Initialize(){
    int i;
    // ---------------load kernel (4 MB )----------------
    // since it's a single 4MB page, its PS is 1
    // P = 1, RW = 1, US = 0, PWT = 0, PCD = 0, A = 0, D = 0, PS = 1
    // 10000011 = 0x83
    // and kernel physical address is 0x400000 (4MB) which is stored above
    page_dic[0] = KERNEL_PHYSICAL_ADDRESS|0x83; 


    //----------------load video memory (4 KB )------------
    // for 0~4MB
    // P = 0, RW = 1, US = 0, PWT = 0, PCD = 0, A = 0, D = 0, PS = 0
    // 00000010
    page_dic[1] = ((uint32_t)page_table)|0x3;
    // here we initialize PTE
    for (i = 0; i < PAGE_TABLE_COUNT; i++ ){
        // i*4 refers to physical address for 4KB memory space
        page_table[i] = (i*4*1024) | 0x2;
    }
    
    //from lib.c we know video starts at 0xB8000 = 753664. 753664/1024/4 = 184
    // P = 1, RW = 1, US = 0, PWT = 0, PCD = 0, A = 0, D = 0, PS = 0
    // 00000011
    page_table[184] = (0xB8000) | 0x3;

    // 1. load page_directory into cr3
    // 2. set CR0 PG bit to 1 (enable paging)
    // 3. set CR4 to map PDE into 4 MB page.
    asm volatile(
        "movl %0, %%eax; \
        movl %%eax, %%cr3; \
        movl %%cr0, %%eax; \
        orl 0x8000000, %%eax; \
        movl %%eax, %%cr0; \
        movl %%cr4, %%eax; \
        orl 0x00000010, %%eax; \
        movl %%eax, %%cr4;"
        : :"r"(page_dic):"%eax"
    );
}

