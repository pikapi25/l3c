#ifndef _PAGING_H
#define _PAGING_H
#include "types.h"


#define VGA_START 0xA0000
// page table and page dictionary all contain 1024 entries
// and each entry is 32 bit, so use uint32_t
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
extern void Page_Initialize();           
extern void mapping_vir_to_phy(uint32_t vir_addr, uint32_t phy_addr);
extern void map_vidmap_page(uint32_t vir_addr, uint32_t phy_addr, int32_t index);
extern void map_video_PTE(uint32_t phy_addr);
extern void update_vidmem_paging(uint8_t term_id);
#endif


