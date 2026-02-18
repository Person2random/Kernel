#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "vga.h"
#include "console.h"
volatile uint32_t ticks = 0;
volatile uint8_t waitmode = 0;
volatile char inputbuf[128] = {};
volatile uint8_t index = 0;
typedef void (*out_cb_t)(char *buf);
uint8_t active_tty = 0;
volatile out_cb_t callbacks[3];

static out_cb_t out = NULL;

void changeout(out_cb_t cb, size_t index) {
    if(index < 3)
    callbacks[index] = cb;
}

void resetout(void) {
    out = NULL;
}


void* memset(void* dst, int v, size_t n) {
    unsigned char* p = dst;
    while (n--) *p++ = (unsigned char)v;
    return dst;
}


int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) return 0; // not equal
        s1++;
        s2++;
    }
    return (*s1 == 0 && *s2 == 0); // return 1 if both ended
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

const char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s',
    'd','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v',
    'b','n','m',',','.','/',0,'*',0,' ', /* rest zero */
};


// kstd.c
void panic(const char *msg) {
      terminal_writestring("KERNEL PANIC: ");
      terminal_writestring(msg);
      terminal_writestring("\n");
      __asm__ volatile("cli");
      while (1) { __asm__ volatile("hlt"); }
}


size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

size_t read_ibuf(uint8_t *buf){
    size_t len = strlen(inputbuf);
    size_t i = 0;
    while(i < len)
    {
        buf[i] = inputbuf[i];
        i++;
    }
    return i;
    
}
void append_ibuf(uint8_t sc)
{
    if(sc == 61){
        terminal_writestring("Switching\n");
        if(active_tty == 2){
            active_tty = 0;
        }
        else {
            active_tty++;
        }
    }
    if(callbacks[active_tty] == NULL) return;

    char ch = scancode_to_ascii[sc];
    

    // Enter pressed -> finalize line
    if (ch == '\n') {
        terminal_writestring("\n");

        callbacks[active_tty](inputbuf);

        memset(inputbuf, 0, 128);
        index = 0;
        return;
    }

    // Backspace (scancode 14 is typical for PS/2 set 1)
    if (sc == 14) {
        if (index > 0) {
            index--;
            inputbuf[index] = 0;
            terminal_removechar();   // assumes this erases last char on screen
        }
        return;
    }

    // Normal character append
    if (index < 127) { // keep room for '\0'
        inputbuf[index++] = ch;
        inputbuf[index] = 0;        // keep it a valid C-string
        terminal_putchar(ch);
    } else {
        // overflow policy: reset (your choice)
        memset(inputbuf, 0, 128);
        index = 0;
    }
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



void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
