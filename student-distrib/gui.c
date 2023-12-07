#include "gui.h"
#include "scheduler.h"
#include "paging.h"
#include "mouse.h"
#include "time.h"
/** init_gui: initialize the GUI
 * Input: None
 * Output: None
 * Side Effect: change the vbe memory
*/
void init_gui(){
    int i;
    init_vga();
    draw_background(BG_COLOR);
    draw_status_bar();
    init_term_bg(0, 10, 30, 0xa7f2ec, STATUS_BAR_FONT_COLOR, 0xFAFAFA, 0x2D2E2E, "term 1");
    init_term_bg(1, 60, 80, 0xa7f2ec, STATUS_BAR_FONT_COLOR, 0xFAFAFA, 0x2D2E2E, "term 2");
    init_term_bg(2, 110, 130, 0xa7f2ec, STATUS_BAR_FONT_COLOR, 0xFAFAFA, 0x2D2E2E, "term 3");
    for(i = 0; i < NUM_TERMS; i++){
        term_orders[i] = i;
    }
    draw_terminals();
    show_screen();
}

/** draw_background: draw the background using given color
 * Input: background color
 * Output: None
 * Side Effect: change fbuf
*/
void draw_background(uint32_t color){
    int i, j;
    for(i = 0; i < VGA_DIMX; i++){
        for(j = 0; j < VGA_DIMY; j++){
            Pdraw(i, j, color);
        }
    }
    Rdraw(SPEAKER_W, SPEAKER_H, SPEAKER_X, SPEAKER_Y, 0);
}
/** draw_status_bar: draw a status bar at the bottom of screen
 * Input: None
 * Output: None
 * Side_effect: change the vbe memory
*/
void draw_status_bar(){
    int i;
    char time_buf[18];
    memset(time_buf, 0, 18);
    put_time_to_buf(time_buf);
    Rdraw(VGA_DIMX, STATUS_BAR_HEIGHT, 0, VGA_DIMY-STATUS_BAR_HEIGHT, STATUS_BAR_COLOR);
    draw_sentence(STATUS_BAR_TERM_START , STATUS_BAR_Y + STATUS_BAR_PADDING, "Terminal 0", STATUS_BAR_FONT_COLOR);
    Rdraw(2, STATUS_BAR_HEIGHT, STATUS_BAR_TERM_START+STATUS_BAR_TERM_W - 2, STATUS_BAR_Y, STATUS_BAR_FONT_COLOR);
    draw_sentence(STATUS_BAR_TERM_START + STATUS_BAR_TERM_W, STATUS_BAR_Y + STATUS_BAR_PADDING, "Terminal 1", STATUS_BAR_FONT_COLOR);
    Rdraw(2, STATUS_BAR_HEIGHT, STATUS_BAR_TERM_START+STATUS_BAR_TERM_W*2 - 2, STATUS_BAR_Y, STATUS_BAR_FONT_COLOR);
    draw_sentence(STATUS_BAR_TERM_START + STATUS_BAR_TERM_W * 2, STATUS_BAR_Y + STATUS_BAR_PADDING, "Terminal 2", STATUS_BAR_FONT_COLOR);
    Rdraw(2, STATUS_BAR_HEIGHT, STATUS_BAR_TERM_START+STATUS_BAR_TERM_W*3 - 2, STATUS_BAR_Y, STATUS_BAR_FONT_COLOR);
    draw_sentence(STATUS_BAR_TIME_START, STATUS_BAR_Y + STATUS_BAR_PADDING, time_buf, STATUS_BAR_FONT_COLOR);

    //draw_sentence(STATUS_BAR_PADDING, STATUS_BAR_Y + STATUS_BAR_PADDING, "Test!", STATUS_BAR_FONT_COLOR);
    //draw_char(STATUS_BAR_PADDING, STATUS_BAR_Y + STATUS_BAR_PADDING, "0", STATUS_BAR_FONT_COLOR);
}

void int_to_char(int num, char* buf, int end_i, int start_i){
    int mod;
    int i = end_i;
    while(num!=0){
        mod = num % 10;
        buf[i--] = (char)(mod + 0x30);
        num /= 10;
    }
    while(i > start_i){
        buf[i--] = '0';
    }
}

void put_time_to_buf(char* buf){
    get_time();
    // int_to_char(year, buf, 3, start);
    // int_to_char(month, buf, 5, 3);
    // int_to_char(day, buf, 7, 5);
    // int_to_char(hour, buf, 9, 7);
    // int_to_char(minute, buf, 11, 9);
    // int_to_char(second, buf, 13, 11);
    int_to_char(year, buf, 3, 0);
    buf[4] = '/';
    int_to_char(month, buf, 6, 4);
    buf[7] = '/';
    int_to_char(day, buf, 9, 7);
    buf[10] = ' ';
    int_to_char(hour, buf, 12, 10);
    buf[13] = ':';
    int_to_char(minute, buf, 15, 13);
    buf[16] = ':';
    int_to_char(second, buf, 18, 16);
}
/** draw_char: draw a character of size 8x16 at given position
 * Input: x - the x coord of character; y - the y coord of character;
 *        ch - the character; color - the color of character
 * Output: None
 * Side Effect: change fbuf
*/
void draw_char(int x, int y, char ch, uint32_t color){
    char* f = (char*)font[(uint8_t)(ch)];
    int set;
    int i,j;
    for (j=0; j < FONT_HEIGHT; j++) {
        for (i=0; i < FONT_WIDTH; i++) {
            set = f[j] & 1 << (8-i);
            if(set) {
                Pdraw(x + i, y + j, color);
            }
        }
    }
}

/** draw_sentence: draw a sentence at given position with given color
 * Input: x, y - the top left position; sentence - the array of character; color - the color
 * Output: None
 * Side Effect: change fbuf
*/
void draw_sentence(int x, int y, char* sentence, uint32_t color){
    int i;
    uint32_t len = strlen(sentence);
    for (i = 0; i < len; i++){
        draw_char(x+i*8, y, sentence[i], color);
    }
}

/** init_term_bg: initialize the terminial windows
 * Input: terminal id, position, colors and title
 * Output: None
 * Side Effect: term_window[id] changes
*/
void init_term_bg(int term_id, int x, int y, uint32_t title_bg_color, uint32_t title_font_color, uint32_t window_bg_color, uint32_t window_font_color, char* title){
    term_window[term_id].x_coord = x;
    term_window[term_id].y_coord = y;
    term_window[term_id].title_bg_color = title_bg_color;
    term_window[term_id].title_font_color = title_font_color;
    term_window[term_id].window_bg_color = window_bg_color;
    term_window[term_id].window_font_color = window_font_color;
    term_window[term_id].terminal_show = 1;
    uint32_t len = strlen(title);
    memset(term_window[term_id].title, 0, 10);
    memcpy(term_window[term_id].title, title, len);
}
/** draw_terminal_bg: draw a terminal at given position, with given title and color
 * Input: terminal id
 * Output: None
 * Side Effect: change fbuf
*/
void draw_terminal_bg(int term_id){
    /* first draw the title bar*/
    Rdraw(WINDOW_WIDTH, WINDOW_TITLE_HEIGHT, term_window[term_id].x_coord, term_window[term_id].y_coord, term_window[term_id].title_bg_color);
    /* Then draw the title*/
    draw_sentence(term_window[term_id].x_coord, term_window[term_id].y_coord,term_window[term_id].title, term_window[term_id].title_font_color);
    /* Next draw the window*/
    Rdraw(WINDOW_WIDTH, WINDOW_HEIGHT, term_window[term_id].x_coord, term_window[term_id].y_coord+WINDOW_TITLE_HEIGHT, term_window[term_id].window_bg_color);

}

void update_screen(){
    draw_background(BG_COLOR);
    draw_terminals();
    draw_status_bar();
    draw_mouse();
    show_screen();
}

/** draw_terminals:draw all terminals with priorities
 * Input: None
 * Output: None
*/
void draw_terminals(){
    int i;
    for (i = NUM_TERMS - 1; i >= 0; i --){
        draw_one_terminal(term_orders[i]);
    }
}

/** update_order: put the term_id-th terminal to the top
 * Input: the id of the top terminal
 * Output: None
 * Side Effect: term_orders changed
*/
void update_order(int term_id){
    int i;
    int term_id_id;
    for (i = 0; i < NUM_TERMS; i++){
        if (term_orders[i] == term_id){
            term_id_id = i;
            break;
        }  
    }
    for (i = term_id_id; i > 0; i--){
        term_orders[i] = term_orders[i-1];
    }
    term_orders[0] = term_id;
}

/** draw_one_terminal: draw one specific terminal
 * Input: terminal id
 * Output: None
 * Side Effect: change fbuf
*/
void draw_one_terminal(int term_id){
    int i, j;
    uint8_t c;
    uint8_t* mem_start;
    //update_vidmem_paging(term_id);
    // if (curr_term_id == term_id){
    //     mem_start = VIDEO;
    // }else{
    //     mem_start = terminal[term_id].background_buffer;
    //     //mem_start = VIDEO;
    // }
    //mem_start = VIDEO;
    if (!term_window[term_id].terminal_show) return;
    mem_start =terminal[term_id].background_buffer;
    draw_terminal_bg(term_id);
    for (i = 0; i < NUM_ROWS; i++){
        for (j = 0; j < NUM_COLS; j++){
            c = *(uint8_t *)(mem_start + ((NUM_COLS * i + j) << 1));
            draw_char(term_window[term_id].x_coord + WINDOW_PAD +  j * FONT_WIDTH, term_window[term_id].y_coord + WINDOW_TITLE_HEIGHT + WINDOW_PAD + i * FONT_HEIGHT, (char)c, term_window[term_id].window_font_color);
        }
    }
    //update_vidmem_paging(myScheduler.cur_task);
}

void draw_mouse(){
    int i, j;
    for (i = 0; i < 16; i++){
        for (j = 0; j < 16; j++){
            if (mouse_basic[i][j] == '*'){
                Pdraw(my_mouse.mouse_x+j, my_mouse.mouse_y+i, 0xFFFFFF);
            }else if(mouse_basic[i][j] == '1'){
                Pdraw(my_mouse.mouse_x+j, my_mouse.mouse_y+i, 0);
            }
        }
    }
}