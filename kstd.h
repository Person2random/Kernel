#ifndef KSTD_H
#define KSTD_H
void* memset(void* dst, int v, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
void wait(int seconds);
void itoa(int value, char* str, int base);
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
#endif