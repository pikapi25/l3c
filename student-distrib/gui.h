#ifndef _GUI_H
#define _GUI_H
#include "font.h"
#include "vga.h"
#include "terminal.h"

#define BG_COLOR 0xEBBCDF

#define STATUS_BAR_PADDING 2
#define STATUS_BAR_HEIGHT 22
#define STATUS_BAR_COLOR 0xD7F3F5
#define STATUS_BAR_FONT_COLOR 0x5F6666
#define STATUS_BAR_Y VGA_DIMY - STATUS_BAR_HEIGHT 

#define WINDOW_PAD 2
#define WINDOW_TITLE_HEIGHT FONT_HEIGHT + 2 * WINDOW_PAD
#define WINDOW_WIDTH    NUM_COLS * FONT_WIDTH + 2 * WINDOW_PAD
#define WINDOW_HEIGHT   NUM_ROWS * FONT_HEIGHT + 2 * WINDOW_PAD


typedef struct{
    int x_coord;
    int y_coord;
    uint32_t title_bg_color;
    uint32_t title_font_color;
    uint32_t window_bg_color;
    uint32_t window_font_color;
    char title[10];
}term_window_t;

term_window_t term_window[NUM_TERMS];

int term_orders[NUM_TERMS];
void init_vga();
void draw_status_bar();
void draw_sentence(int x, int y, char* sentence, uint32_t color);
void draw_char(int x, int y, char ch, uint32_t color);
void init_term_bg(int term_id, int x, int y, uint32_t title_bg_color, uint32_t title_font_color, uint32_t window_bg_color, uint32_t window_font_color, char* title);
void draw_terminal_bg(int term_id);
void draw_terminals();
void update_order(int term_id);
void draw_one_terminal(int term_id);
void update_screen();
void draw_mouse();
#endif