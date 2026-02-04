#include "kstd.h"
#include "vga.h"
/* VGA memory address */
#define VGA_MEMORY 0xB8000

void* mmemset(void* dst, int v, size_t n) {
    unsigned char* p = dst;
    while (n--) *p++ = (unsigned char)v;
    return dst;
}


/* Terminal state */
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;


void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    if(terminal_row == 25){
        terminal_row = 0;
        terminal_column = 0;
        mmemset(terminal_buffer,0,4000);
    }
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_row++;
        terminal_column = 0;
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_row = 0;
            }
        }
    }
}
void terminal_removechar(void){
    if (terminal_column == 0) {
        if (terminal_row == 0) return; // top-left, nothing to remove
        terminal_row--;
        terminal_column = VGA_WIDTH - 1; // 79
    } else {
        terminal_column--;
    }

    // overwrite the previous character with space
    terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}


void femboysay(const char* data) {
    terminal_writestring("Femboy Adham says: ");
    terminal_write(data, strlen(data));
}
