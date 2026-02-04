#include "kstd.h"
#include "vga.h"
void handle_input(char *input){
    size_t len = strlen(input);
    char buf[len+1];
    buf[len] = '\0';
    memcpy(buf,input,len);
    if(strcmp(buf,"quit")){
        terminal_writestring("Shutting down\n");
        volatile int x = 1;
        volatile int y = 0;
        volatile int z = x/y;
        return;
    }
}