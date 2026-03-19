#include <stdint.h>
#include <stddef.h>

#include "tty.h"
#include "kstd.h"
#include "vga.h"
volatile tty ttys[20];
volatile uint8_t index = 0;
volatile uint8_t tty_mode = 0;
uint8_t active_tty = 0;



void ttys_initialize(void) {
    for(int i = 0; i < 20; i++){
        ttys[i].terminal_row = 0;
        ttys[i].terminal_col = 0;
        ttys[i].terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

        for (size_t y = 0; y < VGA_HEIGHT; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                const size_t index = y * VGA_WIDTH + x;
                ttys[i].mem[index] = vga_entry(' ', ttys[i].terminal_color);
            }
        }
    }
}



void tty_putentryat(char c, uint8_t color, size_t x, size_t y,int tty) {
    if (y >= VGA_HEIGHT || x >= VGA_WIDTH) return;
    const size_t index = y * VGA_WIDTH + x;
    ttys[tty].mem[index] = vga_entry(c,color);
}

void tty_move_cursor(uint16_t row, uint16_t col, int tty){
    ttys[tty].terminal_row = row;
    ttys[tty].terminal_col = col;
}

void tty_scroll(int tty) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            ttys[tty].mem[y * VGA_WIDTH + x] = ttys[tty].mem[(y + 1) * VGA_WIDTH + x];
        }
    }
    const size_t last = (VGA_HEIGHT - 1) * VGA_WIDTH;
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        ttys[tty].mem[last + x] = vga_entry(' ', ttys[tty].terminal_color);
    }
}


void tty_putchar(char c,int tty) {
    if (c == '\n') {
        ttys[tty].terminal_row++;
        ttys[tty].terminal_col = 0;
        if (ttys[tty].terminal_row == VGA_HEIGHT) {
            tty_scroll(tty);
            ttys[tty].terminal_row = VGA_HEIGHT - 1;
        }
    } else {
        tty_putentryat(c, ttys[tty].terminal_color, ttys[tty].terminal_col, ttys[tty].terminal_row,tty);
        if (++ttys[tty].terminal_col == VGA_WIDTH) {
            ttys[tty].terminal_col = 0;
            if (++ttys[tty].terminal_row == VGA_HEIGHT) {
                tty_scroll(tty);
                ttys[tty].terminal_row = VGA_HEIGHT - 1;
            }
        }
    }
    tty_move_cursor(ttys[tty].terminal_row,ttys[tty].terminal_col,tty);
}





void tty_write(const char* data, size_t size, int tty) {
    for (size_t i = 0; i < size; i++) {
        tty_putchar(data[i],tty);
    }
}

void tty_writestring(const char* data,int tty) {
    tty_write(data, strlen(data),tty);
}





void ttyprint(const char* s, int tty){
    tty_writestring(s,tty);
}


void save_terminal(){
    memcpy(ttys[active_tty].inputbuf,inputbuf,128);
    memcpy(ttys[active_tty].mem,(void*)0xB8000,4000);
    ttys[active_tty].terminal_row = vga_get_row();
    ttys[active_tty].terminal_col = vga_get_col();
}

void show_terminal(){
    memcpy((void*)0xB8000,ttys[active_tty].mem,4000);
    memcpy(inputbuf,ttys[active_tty].inputbuf,128);

    vga_move_cursor(0,0);
    vga_set_col(0);
    vga_set_row(0);

    if(ttys[active_tty].terminal_row == 0){
        vga_move_cursor(ttys[active_tty].terminal_row+1,ttys[active_tty].terminal_col);
        vga_set_row(ttys[active_tty].terminal_row+1);

    }
    else{
        vga_move_cursor(ttys[active_tty].terminal_row,ttys[active_tty].terminal_col);

        vga_set_row(ttys[active_tty].terminal_row);
    }
    vga_set_col(ttys[active_tty].terminal_col);
    terminal_writestring(" ");
    terminal_removechar();


}



void append_ibuf(uint8_t sc)
{
    if(sc == 61){
        save_terminal();
        if(active_tty == 0){
            active_tty = 19;
        }
        else{
            active_tty--;
        }
        show_terminal();
        return;
    }
    else if(sc == 62){
        save_terminal();
        if(active_tty == 19){
            active_tty = 0;
        }
        else{
            active_tty++;
        }
        show_terminal();

        return;
    }
    char ch = scancode_to_ascii[sc];
    
    // Enter pressed -> finalize line
    if (ch == '\n') {
        terminal_writestring("\n");
        if(ttys[active_tty].cb == NULL) return;
        ttys[active_tty].cb(inputbuf);

        memset(inputbuf, 0, 128);
        index = 0;
        return;
    }

    // Backspace (scancode 14 is typical for PS/2 set 1)
    if (sc == 14) {
        if (index > 0) {
            index--;
            inputbuf[index] = 0;
            terminal_removechar();   // assumes this erases last char on screen
        }
        return;
    }

    // Normal character append
    if (index < 127) { // keep room for '\0'
        inputbuf[index++] = ch;
        inputbuf[index] = 0;        // keep it a valid C-string
        terminal_putchar(ch);
    } else {
        // overflow policy: reset (your choice)
        memset(inputbuf, 0, 128);
        index = 0;
    }
}


