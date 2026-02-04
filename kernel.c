#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "gdt.h"
#include "isr.h"
#include "idt.h"
#include "kstd.h"


uint8_t kernel_stack[16384];
extern uint8_t kernel_stack[];
void kernel_main(void){
    terminal_initialize();
    femboysay("Will init GDT\n");
    gdt_init();
    femboysay("GDT initialized\n");
    femboysay("Will init IDT\n");
    idt_init();
    femboysay("IDT initialized\n");
    femboysay("Will remap PIC\n");
    //pic_remap();
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
    femboysay("Interrupts enabled \n");
    volatile int x = 1/0;
    while (1)
    {
        __asm__ volatile("hlt"); 
    }
    
}