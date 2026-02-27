#include "kstd.h"
#include <stdint.h>
extern uint8_t _kernel_end[];
uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));
#define HEAP_BASE 0x01000000   // 16MB
#define HEAP_LIMIT 0x04000000  // 64MB
static uint32_t placement = 0;
static uint32_t heap_brk = HEAP_BASE;


void* kmalloc(uint32_t size) {
    uint32_t addr = heap_brk;
    heap_brk += size;
    return (void*)addr;
}


void init_placement_allocator() {
    placement = ((uint32_t)_kernel_end + 0x3FFFFF) & ~0x3FFFFF; // align to 4MB
}

uint32_t alloc_4mb_chunk() {
    uint32_t addr = placement;
    placement += 0x400000;
    return addr;
}

void init_paging(void) 
{
    // Fill first page table (0–4MB identity)
    for (int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 0x3; // present + RW
    }

    // Set first PDE to point to page table
    page_directory[0] = ((uint32_t)first_page_table) | 0x3;

    // Clear rest
    for (int i = 1; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // Load CR3
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory));

    // Enable paging
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}