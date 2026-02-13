#include "kstd.h"
#include "vga.h"
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
    terminal_writestring("Invalid command\n");
}