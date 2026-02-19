#include "kstd.h"
#include "vga.h"
extern uint8_t _kernel_end[];
extern uint8_t _kernel_start[];


void handle_shell(char *input){
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
    if(strcmp(buf,"nigger")){
        terminal_writestring("Syfm youre getting your terminal rights revoked\n");
        changeout(NULL,0);
        return;
    }

    char* tokens[20];
    size_t count = split(input,' ',tokens,20);
    if(count > 0 && strcmp(tokens[0],"current")){
            char a[2];
            itoa(active_tty,a,10);
            terminal_writestring(a);
            terminal_writestring("\n");
            return;
    }
    
    if (count > 0 && strcmp(tokens[0], "shell")) {
        if (count < 2) {
            terminal_writestring("Usage: <num>");
            return;
        }
        int shell_num = stoia(tokens[1]);
        if (shell_num > 2) {
            terminal_writestring("Invalid argument\n");
            return;
        }
        changeout(handle_shell, shell_num);
        return;
    }
    

    
    terminal_writestring("Invalid command\n");
}