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



//Will add IDT implementation

//TODO: Put IDT in different .h and .c files to avoid clutter
struct idt_entry {
    uint16_t offset_low;   // lower 16 bits of handler address
    uint16_t selector;     // kernel code segment selector
    uint8_t  zero;         // always 0
    uint8_t  type_attr;    // flags
    uint16_t offset_high;  // upper 16 bits of handler address
} __attribute__((packed));


struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

#define IDT_ENTRIES 256

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;
extern void idt_load(uint32_t);
extern void isr0(void);

//Function to set one entry
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].offset_low  = base & 0xFFFF;
    idt[num].selector    = selector;
    idt[num].zero        = 0;
    idt[num].type_attr   = flags;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
}



void idt_init() {
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    // clear table
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);

    idt_load((uint32_t)&idtp);
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
    terminal_writestring("IDT initialized\n");
    
    
    while (1)
    {
        __asm__ volatile("hlt");
    }
    
}