#include "idt.h"
#include "vga.h"
#include "gdt.h"   // if needed
#include "kstd.h"
#include "paging.h"
#include "pmm.h"
// Declare all 32 ISR stubs from your assembly file
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

// Array of stubs
static void (*isr_routines[32])(void) = {
    isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7,
    isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15,
    isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
};

static inline void flush_tlb(void)
{
    uint32_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    __asm__ volatile("mov %0, %%cr3" :: "r"(cr3));
}

// Initialize IDT entries 0–31
void isr_init(void) {
    for (uint8_t i = 0; i < 32; i++) {
        idt_set_gate(i, (uint32_t)isr_routines[i], 0x08, 0x8E);
    }
}

// Minimal ISR handler
void isr_handler(int num, uint32_t err) {
    terminal_writestring("Kernel interrupt: ");
    char buf[16];
    itoa(num, buf, 10);
    terminal_writestring(buf);

    if (num == 14) {
        uint32_t cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        terminal_writestring(" PF addr=0x");
        itoa((int)cr2, buf, 16);
        terminal_writestring(buf);
        terminal_writestring(" err=0x");
        itoa((int)err, buf, 16);
        terminal_writestring(buf);

        terminal_writestring(" [");
        terminal_writestring((err & 0x1) ? "P" : "NP");
        terminal_writestring((err & 0x2) ? "|W" : "|R");
        terminal_writestring((err & 0x4) ? "|U" : "|S");
        terminal_writestring((err & 0x8) ? "|RSVD" : "");
        terminal_writestring((err & 0x10) ? "|IF" : "");
        terminal_writestring("]\n");

        if (cr2 >= HEAP_BASE && cr2 < HEAP_LIMIT) {
            uint32_t fault_addr = cr2;

            uint32_t pde_index = fault_addr >> 22;
            uint32_t pte_index = (fault_addr >> 12) & 0x3FF;

            if (!(page_directory[pde_index] & 1)) {
                uint32_t pt_phys = pmm_alloc_frame();
                memset((void*)pt_phys, 0, 4096);

                page_directory[pde_index] = pt_phys | 0x3;
            }

            uint32_t* page_table =
                (uint32_t*)(page_directory[pde_index] & 0xFFFFF000);

            if (!(page_table[pte_index] & 1)) {
                uint32_t page_phys = pmm_alloc_frame();
                page_table[pte_index] = page_phys | 0x3;
            }

            flush_tlb();
            return;
        }
    }

    terminal_writestring("\n");
    while (1) { __asm__ volatile("hlt"); }
}
