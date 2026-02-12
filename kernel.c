#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "gdt.h"
#include "isr.h"
#include "idt.h"
#include "kstd.h"
#include "paging.h"

extern uint8_t kernel_stack[];
// GDT flush expects this symbol; reserve a 16 KiB kernel stack.
uint8_t kernel_stack[16384] __attribute__((aligned(16)));
void kernel_main(void){
    terminal_initialize();
    terminal_writestring("Will init GDT\n");
    gdt_init();
    terminal_writestring("GDT initialized\n");
    terminal_writestring("Will init IDT\n");
    idt_init();
    isr_init();
    terminal_writestring("IDT initialized\n");
    terminal_writestring("Will remap PIC\n");
    pic_remap();
    terminal_writestring("PIC remapped\n");
    terminal_writestring("Will unmask/mask\n");
    outb(0x21, 0xFC); // unmask timer + keyboard
    outb(0xA1, 0xFF); // mask all IRQs on slavep
    terminal_writestring("Done.\n");
    terminal_writestring("Will change PIT divisor\n");
    uint16_t divisor = 1193;
    outb(0x43,0x36);
    outb(0x40,divisor & 0xFF);
    outb(0x40,(divisor >> 8)&0xFF);
    waitmode = 1;
    terminal_writestring("Accurate timing implemented\n");
    terminal_writestring("Will enable interrupts\n");
    __asm__ volatile("sti");
    wait(1);
    terminal_writestring("Interrupts enabled \n");
    init_paging();
    while (1)
    {
        __asm__ volatile("hlt"); 
    }
    
}
