#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "gdt.h"
#include "isr.h"
#include "idt.h"
#include "kstd.h"
#include "paging.h"
#include "console.h"

extern uint8_t kernel_stack[];
// GDT flush expects this symbol; reserve a 16 KiB kernel stack.
uint8_t kernel_stack[16384] __attribute__((aligned(16)));
void kernel_main(void){
    uint64_t max[400000];
    terminal_initialize();
    femboysay("Welcome to eOS, Epstein OS\n");
    femboysay("Will init GDT\n");
    gdt_init();
    femboysay("GDT initialized\n");
    femboysay("Will init IDT\n");
    idt_init();
    isr_init();
    femboysay("IDT initialized\n");
    femboysay("Will remap PIC\n");
    pic_remap();
    femboysay("PIC remapped\n");
    femboysay("Will unmask/mask\n");
    outb(0x21, 0xFC); // unmask timer + keyboard
    outb(0xA1, 0xFF); // mask all IRQs on slavep
    femboysay("Done.\n");
    femboysay("Will change PIT divisor\n");
    uint16_t divisor = 1193;
    outb(0x43,0x36);
    outb(0x40,divisor & 0xFF);
    outb(0x40,(divisor >> 8)&0xFF);
    waitmode = 1;
    femboysay("Accurate timing implemented\n");
    femboysay("Will enable interrupts\n");
    __asm__ volatile("sti");
    wait(1);
    femboysay("Interrupts enabled\n");
    femboysay("Will init paging\n");
    init_paging();
    femboysay("Paging init\n");
    changeout(handle_shell,0);
    while (1)
    {
        __asm__ volatile("hlt"); 
    }
    
}
