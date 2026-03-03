#ifndef PAGING_H
#define PAGING_H

#define HEAP_BASE 0x01000000   // 16MB
#define HEAP_LIMIT 0x04000000
extern uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t alloc_4mb_chunk();
void init_paging(void);
void* kmalloc(uint32_t size);
#endif
