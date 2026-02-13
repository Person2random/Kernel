#include "kstd.h"
#include "vga.h"
extern uint8_t _kernel_end[];
extern uint8_t _kernel_start[];


void handle_input(char *input){
    size_t len = strlen(input);
    char buf[len+1];
    buf[len] = '\0';
    memcpy(buf,input,len);
    if(strcmp(buf,"hi")){
        terminal_writestring("Hello, World\n");
        return;
    }
    if(strcmp(buf,"yousef")){
        terminal_writestring("Yousef is 5awal and metnak\n");
        return;
    }
    if(strcmp(buf,"ksize")){
        uint32_t diff = (uint32_t) _kernel_end - (uint32_t)_kernel_start;
        char bbuf[12];
        itoa(diff,bbuf,10);
        terminal_writestring("Kernel is ");
        terminal_writestring(bbuf);
        terminal_writestring(" Bytes big\n");
        return;
    }

    
    terminal_writestring("Invalid command\n");
}