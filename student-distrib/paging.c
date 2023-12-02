#include "paging.h"
#include "types.h"
#include "terminal.h"

#define PAGE_TABLE_COUNT    1024
#define PAGE_DIC_COUNT  1024
#define KERNEL_PHYSICAL_ADDRESS 0x400000
#define KERNEL_PHYSICAL_ADDRESS_END 0x7FFFFF
#define fourKB 4096
#define NotPresent 0
#define video_memory_start 0xb8000
#define video_memory_end 0xb9fff
#define user_paging_set 0x87 // PS, U/S, R/W, P = 1
#define video_page_set 0x7 //U/S,R/W,P=1
#define user_PTE_set 0x3 //present = 1, R/W = 1
#define video_PTE_set 0x7 //present = 1, R/W = 1, privilege = 1
#define page_dir_start 22
#define page_table_start 12
#define USER_PROGRAM_END     0x8800000
uint32_t page_table[PAGE_TABLE_COUNT] __attribute__((aligned (4 * PAGE_TABLE_COUNT)));        // page_table for 0~4 MB
uint32_t page_dic[PAGE_DIC_COUNT] __attribute__((aligned (4 * PAGE_DIC_COUNT))); 
uint32_t video_page_table[PAGE_TABLE_COUNT] __attribute__((aligned (4 * PAGE_TABLE_COUNT)));
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
    // here we initialize all PDE and PTE and make them NOT present
    int i;
    for (i = 0; i < PAGE_TABLE_COUNT; i++ ){
        // i*4 refers to physical address for 4KB memory space
        // 0x0 since these entries are not present 
        page_table[i] = (i*fourKB) | 0x2;
        page_dic[i] = 0x2;
    }

    //----------------load video memory (4 KB )------------
    //from lib.c we know video starts at 0xB8000 = 753664. 753664/1024/4 = 184
    // P = 1, RW = 1, US = 0, PWT = 0, PCD = 0, A = 0, D = 0, PS = 0
    // 00000011
    page_table[184] = 0xB8000|0x3;
    // for 0~4MB
    // P = 1, RW = 1, US = 0, PWT = 0, PCD = 0, A = 0, D = 0, PS = 0
    // 00000010
    page_dic[0] = ((unsigned int)page_table)|0x3;

    // ---------------load kernel (4 MB )----------------
    // since it's a single 4MB page, its PS is 1
    // P = 1, RW = 1, US = 0, PWT = 0, PCD = 0, A = 0, D = 0, PS = 1
    // 10000011 = 0x83
    // and kernel physical address is 0x400000 (4MB) which is stored above
    page_dic[1] = KERNEL_PHYSICAL_ADDRESS|0x83; 

    // 1. load page_directory into cr3
    // 2. set CR0 PG bit to 1 (enable paging) (or 0x80000001)
    // 3. set CR4 to map PDE into 4 MB (or 0x00000010) page.
    asm volatile(
        "movl %0, %%eax; \
        movl %%eax, %%cr3; \
        movl %%cr4, %%eax; \
        orl $0x00000010, %%eax; \
        movl %%eax, %%cr4; \
        movl %%cr0, %%eax; \
        orl $0x80000001, %%eax; \
        movl %%eax, %%cr0; "
        : :"r"(&page_dic)
        : "%eax"
    );

}

/* mapping_vir_to_phy
 * Input: virtual address, physical address
 * Output: None
 * Effect: set page directory to map from virtual to physical
*/
void mapping_vir_to_phy(uint32_t vir_addr, uint32_t phy_addr){
    page_dic[vir_addr>>page_dir_start] = phy_addr | user_paging_set;
    asm volatile(                           
        "movl %%cr3, %%eax;"           
        "movl %%eax, %%cr3;"           
        :
    );                                      
}
/* map_vidmap_page
 * input: virtual_address and physical_address and index
 * output: none
 */ 
void map_vidmap_page(uint32_t vir_addr, uint32_t phy_addr, int32_t index){
    page_dic[(vir_addr)>>page_dir_start] = (unsigned int)(video_page_table)|video_page_set;
    video_page_table[index] = phy_addr|user_paging_set;
    asm volatile(                           
        "movl %%cr3, %%eax;"           
        "movl %%eax, %%cr3;"           
        :
    );      
}

void map_video_PTE(uint32_t phy_addr){
    page_table[(video_memory_start)>>page_table_start] = phy_addr|user_PTE_set;
    video_page_table[(USER_PROGRAM_END<<10)>>22] = phy_addr|video_PTE_set;
    asm volatile(                           
        "movl %%cr3, %%eax;"           
        "movl %%eax, %%cr3;"           
        :
    );   
}

void update_vidmem_paging(uint8_t term_id){
    /* if it's current terminal, map to main video memory */
    if (curr_term_id == term_id){
        map_video_PTE(VIDEO_MEM_LOC);
    }else{
        /* otherwise map to background buffer*/
        map_video_PTE((uint32_t)terminal[term_id].background_buffer);
    }
}
