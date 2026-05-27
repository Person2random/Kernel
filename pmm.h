#pragma once
#include <stdint.h>

void pmm_init(uint32_t mb_info_ptr);
uint32_t pmm_alloc_frame(void);          // returns physical addr (4KB aligned) or 0
void pmm_free_frame(uint32_t phys_addr); // phys_addr must be 4KB aligned