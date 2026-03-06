#include "pmm.h"
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
#include "multiboot.h"
#include "tty.h"
extern uint8_t kernel_stack[];
// GDT flush expects this symbol; reserve a 16 KiB kernel stack.
uint8_t kernel_stack[16384] __attribute__((aligned(16)));
void kernel_main(uint32_t magic, void* mb_info){
    terminal_initialize();
    if (magic != 0x2BADB002 && magic != 0x36D76289) {
        terminal_writestring("Unknown boot magic, halting.\n");
        while (1) __asm__ volatile("hlt");
    }
    pmm_init((uint32_t)mb_info);

    uint32_t a = pmm_alloc_frame();
    uint32_t b = pmm_alloc_frame();
    uint32_t c = pmm_alloc_frame();


    pmm_free_frame(b);
    uint32_t d = pmm_alloc_frame();
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
    kwait(1);
    femboysay("Interrupts enabled\n");
    femboysay("Will init paging\n");
    init_paging();
    femboysay("Paging init\n");
    uint32_t* heap_test = (uint32_t*)0x40000000;
    *heap_test = 1234;
    terminal_writestring("Heap page allocated\n");
    setin(0,handle_shell);
    
    while (1)
    {
        __asm__ volatile("hlt"); 
    }
    
}
