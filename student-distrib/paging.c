#include "paging.h"
// #include "types.h"


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

