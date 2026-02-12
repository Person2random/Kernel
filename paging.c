#include "kstd.h"
#include <stdint.h>
extern uint8_t _kernel_end[];

uint32_t page_directory[1024] __attribute__((aligned(4096)));

void init_paging(void) {
	if((uint32_t)_kernel_end >=  0x00400000 ){
		panic("Kernel too big");		
	}
    page_directory[0] = 0x00000083; // Present=1, RW=1, PS=1

    // Clear rest
    for (int i = 1; i < 1024; i++) {
        page_directory[i] = 0;
    }

    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory));

    uint32_t cr0;
    uint32_t cr4;
    __asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 4); // PSE: allow 4MB pages
    __asm__ volatile("mov %0, %%cr4" :: "r"(cr4));

    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // set PG bit
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}
