#include "paging.h"
#include "types.h"
#include "terminal.h"
#include "vbe.h"


#define PAGE_TABLE_COUNT    1024
#define PAGE_DIC_COUNT  1024
#define KERNEL_PHYSICAL_ADDRESS 0x400000
#define KERNEL_PHYSICAL_ADDRESS_END 0x7FFFFF
#define fourKB 4096
#define NotPresent 0
#define video_memory_start VIDEO
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
    uint32_t vbe_mem = (REG_VBE >> 22);
    uint32_t vbe_next_mem = vbe_mem + 1;
    for (i = 0; i < PAGE_TABLE_COUNT; i++ ){
        // i*4 refers to physical address for 4KB memory space
        // 0x0 since these entries are not present 
        page_table[i] = (i*fourKB) | 0x2;
        page_dic[i] = 0x2;
    }

    /* ----- SVGA_MEM_CHANGE ----- */
    //----------------load video memory (4 KB )------------
    //from lib.c we know video starts at 0xB8000 = 753664. 753664/1024/4 = 184
    // P = 1, RW = 1, US = 0, PWT = 0, PCD = 0, A = 0, D = 0, PS = 0
    // 00000011
    page_table[184] = (SVGA_MEM_LOC+ VIDEO_MEM_SIZE*2)|0x3;
    page_table[(VIDEO_MEM_LOC>>page_table_start)+2] = (SVGA_MEM_LOC+ VIDEO_MEM_SIZE*2)|0x3;
    page_table[(VIDEO_MEM_LOC>>page_table_start)+3] = (SVGA_MEM_LOC+ VIDEO_MEM_SIZE*3)|0x3;
    page_table[(VIDEO_MEM_LOC>>page_table_start)+4] = (SVGA_MEM_LOC+ VIDEO_MEM_SIZE*4)|0x3;
    // for 0~4MB
    // P = 1, RW = 1, US = 0, PWT = 0, PCD = 0, A = 0, D = 0, PS = 0
    // 00000010
    page_dic[0] = ((unsigned int)page_table)|0x3;

    // ---------------load kernel (4 MB )----------------
    // since it's a single 4MB page, its PS is 1
    // P = 1, RW = 1, US = 0, PWT = 0, PCD = 0, A = 0, D = 0, PS = 1
    // 10000011 = 0x83
    // and kernel physical address is 0x400000 (4MB) which is stored above
    page_dic[1] = KERNEL_PHYSICAL_ADDRESS|0x183; 

    // ---------------load kernel (4 MB )----------------
    // since it's a single 4MB page, its PS is 1
    // P = 1, RW = 1, US = 0, PWT = 0, PCD = 0, A = 0, D = 0, PS = 1
    // 10000011 = 0x83
    // and kernel physical address is 0x400000 (4MB) which is stored above
    page_dic[vbe_mem] = REG_VBE|0x83; 
    page_dic[vbe_next_mem] = (vbe_next_mem << 22) | 0x183;

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

    asm volatile(                           
        "movl %%cr3, %%eax;"           
        "movl %%eax, %%cr3;"           
        :
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

// ------------helper function-------------
// map_video_PTE
// Input: uint32_t phy_addr
// Function: remap virtual video address (0xb8000) and (136 MB) to provided physical address 
// OutPut: none
// Side effect: modify page_table and video_page_table 
void map_video_PTE(uint32_t phy_addr){
    page_table[(video_memory_start)>>page_table_start] = phy_addr|user_PTE_set;
    // here we left shift 10 to clear first 10 bits and get page table index by right shift 10 + 12 bits
    video_page_table[(USER_PROGRAM_END<<10)>>page_dir_start] = phy_addr|video_PTE_set;
    asm volatile(                           
        "movl %%cr3, %%eax;"           
        "movl %%eax, %%cr3;"           
        :
    );   
}


// update_vidmem_paging
// Input: term_id      
// Function: remap video memory page 
//           if the term_id == curr_term_id, then write directly into Video memory else write into corresponding backup buffer
// OutPut: None
// Side effect: Change backup buffer content 
void update_vidmem_paging(uint8_t term_id){
    /* if it's current terminal, map to main video memory */
    /* ----- SVGA CHANGES ----- */
    // if (curr_term_id == term_id){
    //     map_video_PTE(SVGA_MEM_LOC);
    // }else{
    //     /* otherwise map to background buffer*/
    //     map_video_PTE((uint32_t)terminal[term_id].background_buffer);
    // }
    map_video_PTE((SVGA_MEM_LOC+ VIDEO_MEM_SIZE*(term_id+2)));
}
