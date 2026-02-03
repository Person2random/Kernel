#include "kstd.h"
#include "idt.h"
#include "vga.h"
#include <stdint.h>
struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;
extern void idt_load(uint32_t);
extern void irq0(void);
extern void irq1(void);

#define PIC1        0x20
#define PIC2        0xA0
#define PIC1_DATA   0x21
#define PIC2_DATA   0xA1

void pic_remap(void) {
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    outb(PIC1, 0x11);
    outb(PIC2, 0x11);

    outb(PIC1_DATA, 0x20); // IRQ 0–7 → 32–39
    outb(PIC2_DATA, 0x28); // IRQ 8–15 → 40–47

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}


extern void irq0_handler(void);
extern void irq1_handler(void);




void irq1_handler(void) {
    uint8_t sc = inb(0x60);
    terminal_putchar('K'); // or print scancode
    outb(PIC1,0x20);
}

void irq0_handler(void) {
    ticks++;
    outb(PIC1,0x20);
}



void idt_init(void) {
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    // Clear table
    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_gate(i, 0, 0, 0);

    // ISR0–ISR31 will be set later by isr_init()
    idt_load((uint32_t)&idtp);
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E); // Timer
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E); // Keyboard

}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].offset_low  = base & 0xFFFF;
    idt[num].selector    = selector;
    idt[num].zero        = 0;
    idt[num].type_attr   = flags;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
}
