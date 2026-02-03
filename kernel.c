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
    pic_remap();
    femboysay("PIC remapped\n");
    femboysay("Will unmask/mask\n");
    outb(0x21, 0xFC); // unmask timer + keyboard
    outb(0xA1, 0xFF); // mask all IRQs on slavep
    femboysay("Done.\n");
    femboysay("Will enable interrupts\n");
    __asm__ volatile("sti");
    femboysay("Interrupts enabled \n");
    while (1)
    {
        __asm__ volatile("hlt"); 
    }
    
}