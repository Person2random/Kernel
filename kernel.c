#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "gdt.h"
#include "isr.h"
#include "idt.h"
//Keep in kernel.c will be useful
//Convert integer to any base


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



void iitoa(int value, char* str, int base) {
    char* ptr = str, *ptr1 = str, tmp_char;
    int tmp_value;

    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }

    while (value != 0) {
        tmp_value = value % base;
        *ptr++ = (tmp_value < 10) ? (tmp_value + '0') : (tmp_value - 10 + 'A');
        value /= base;
    }
    *ptr-- = '\0';

    // reverse the string
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
}



uint8_t kernel_stack[16384];
extern uint8_t kernel_stack[];
void kernel_main(void){
    terminal_initialize();
    terminal_writestring("Will init GDT\n");
    gdt_init();
    terminal_writestring("GDT initialized\n");
    terminal_writestring("Will init IDT\n");
    idt_init();
    isr_init();
    int i = 0;
    while (1)
    {
        char buf[20];
        iitoa(i,buf,10);
        terminal_writestring(buf);
        terminal_writestring("\n");
        i++;
    }
    
    

    while (1)
    {
        __asm__ volatile("hlt");
    }
    
}