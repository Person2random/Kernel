#include <stdint.h>
#include <stddef.h>
#ifndef KSTD_H
#define KSTD_H
int strcmp(const char *s1, const char *s2);
extern volatile uint64_t ticks;
extern volatile uint8_t waitmode;
size_t strlen(const char* str);
size_t readbuf(uint8_t *buf);
void append_ibuf(uint8_t c);
void* memset(void* dst, int v, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
void wait(int seconds);
void itoa(int value, char* str, int base);
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void set_kernel_stack();
void panic(const char *msg) __attribute__((noreturn));
#endif