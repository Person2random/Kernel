#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vga.h"




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

void kernel_main(void){
    terminal_initialize();
    terminal_writestring("Hello, World\n");
}