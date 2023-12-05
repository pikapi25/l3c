#include "vbe.h"
#include "gui.h"
#include "uiuc.h"
uint32_t* mem_start = (uint32_t *) QEMU_BASE_ADDR;


/*
 * void draw_uiuc()
 * Input: none
 * Return value: none
 * Function: draw the UIUC background for boot animation
 */
void draw_uiuc() {
    int i,j;
    for (i = 0; i < 130; i++) {
        for (j = 0; j < 420; j++) {
            uint32_t rgb = BLACK;
            rgb |= ((((uiuc[j + i * 420] & RED_MASK) >> 11) << 3) & 0xFF) << 16;
            rgb |= ((((uiuc[j + i * 420] & GREEN_MASK) >> 5) << 2) & 0xFF) << 8;
            rgb |= (((uiuc[j + i * 420] & BLUE_MASK) << 3) & 0xFF);
            if (rgb >= 0x00F85CF8)
                continue;
            *(uint32_t *)(mem_start + j + 300 + (i + 400) * BG_WIDTH) = rgb;
        }
    }
}
