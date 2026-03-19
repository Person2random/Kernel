#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "vga.h"
#include "console.h"
#include "tty.h"
volatile uint32_t ticks = 0;
volatile uint8_t waitmode = 0;
volatile char inputbuf[128] = {};


void* memcpy(void* dst, const void* src, size_t n) {
    unsigned char* d = dst;
    const unsigned char* s = src;
    while (n--) *d++ = *s++;
    return dst;
}


void setin(size_t index, in_cb_t cb){
    ttys[index].cb = cb;
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

int stoi(const char* buf){
    int i = 0;
    int num = 0;
    while (buf[i])
    {
        if (buf[i] == '0' || buf[i] == '1' || buf[i] == '2' || buf[i] == '3' || buf[i] == '4' || buf[i] == '5' || buf[i] == '6' || buf[i] == '7' || buf[i] == '8' || buf[i] == '9' )
        {
            int val = buf[i] - 48;
            num = num * 10 + val; 
        }
        i++;
    }
    return num;
    
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

// Assumes a single-character delimiter.
/**
 * Splits a string in place based on a single character delimiter.
 * The original string is modified by inserting null terminators.
 *
 * @param str The input string (char array) to be split. This is modified.
 * @param delimiter The character used as the split point.
 * @param tokens An array to store pointers to the start of each token.
 * @param max_tokens The maximum number of tokens to find.
 * @return The actual number of tokens found.
 */
int split(char* str, char delimiter, char* tokens[], int max_tokens) {
    int token_count = 0;
    char* p = str;
    char* start = str;

    // Iterate through the string until the end of the string (null terminator) is reached
    while (*p && token_count < max_tokens) {
        if (*p == delimiter) {
            *p = '\0'; // Replace delimiter with null terminator to end the current token
            tokens[token_count++] = start; // Store the start of the token
            start = p + 1; // The next token starts after the delimiter
        }
        p++;
    }

    // Store the last token if there is space
    if (token_count < max_tokens && start <= p) {
        tokens[token_count++] = start;
    }
    
    return token_count;
}


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


//Depends on waitmode.
//If waitmode is set to 0 it will count assuming the speed is 18.2 Hz
//If waitmode is set to 1,The int passed = number of milliseconds to wait
void kwait  (int seconds){
    if(waitmode == 0){
        terminal_writestring("Waiting using waitmode 0 is decaprecated");
        return;
    }
    else if (waitmode == 1)
    {
        uint64_t current = ticks;
        uint64_t target = current+seconds;
        while (ticks < target)
        {

            __asm__ volatile ("sti;hlt");
            
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
