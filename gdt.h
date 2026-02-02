#ifndef GDT_H
#define GDT_H

#include <stdint.h>

struct gdt_entry {
    uint16_t limit_low;     // lower 16 bits of limit
    uint16_t base_low;      // lower 16 bits of base
    uint8_t  base_middle;   // middle 8 bits of base
    uint8_t  access;        // access flags
    uint8_t  granularity;   // granularity + upper 4 bits of limit
    uint8_t  base_high;     // upper 8 bits of base
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void gdt_flush(uint32_t);

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void gdt_init(void);

#endif
