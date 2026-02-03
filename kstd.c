#include <stdint.h>
#include <stddef.h>
#include "idt.h"
volatile uint64_t ticks = 0;
volatile uint8_t waitmode = 0;
void* memset(void* dst, int v, size_t n) {
    unsigned char* p = dst;
    while (n--) *p++ = (unsigned char)v;
    return dst;
}

void* memcpy(void* dst, const void* src, size_t n) {
    unsigned char* d = dst;
    const unsigned char* s = src;
    while (n--) *d++ = *s++;
    return dst;
}


//Depends on waitmode.
//If waitmode is set to 0 it will count assuming the speed is 18.2 Hz
//If waitmode is set to 1,The int passed = number of milliseconds to wait
void wait(int seconds){
    if(waitmode == 0){
        uint64_t current = ticks;
        uint64_t target = 18.2*seconds + current;
        while (1)
        {
            if(ticks >= target){
                break;
            }
        }
    }
    else if (waitmode == 1)
    {
        uint64_t current = ticks;
        uint64_t target = current+seconds;
        while (1)
        {
            if (ticks >= target)
            {
                break;
            }
            
        }
           
    }
    
}


void itoa(int value, char* str, int base) {
    char* ptr = str, *ptr1 = str, tmp_char;
    int tmp_value;
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }

    while (value != 0) {
        tmp_value = value % base;
        *ptr++ = (tmp_value < 10) ? (tmp_value + '0') : (tmp_value - 10 + 'A');
        value /= base;
    }
    *ptr-- = '\0';

    // reverse the string
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
}

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
