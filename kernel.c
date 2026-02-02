#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "gdt.h"



//important memory functions i probably dont understand
void* memset(void* dst, int v, size_t n) {
    unsigned char* p = dst;
    while (n--) *p++ = (unsigned char)v;
    return dst;
}

void* memcpy(void* dst, const void* src, size_t n) {
    unsigned char* d = dst;
    const unsigned char* s = src;
    while (n--) *d++ = *s++;
    return dst;
}


uint8_t kernel_stack[16384];
extern uint8_t kernel_stack[];

void kernel_main(void){
    terminal_initialize();
    terminal_writestring("Will init GDT\n");
    gdt_init();
    terminal_writestring("GDT initialized\n");
    
    while (1)
    {
        __asm__ volatile("hlt");
    }
    
}