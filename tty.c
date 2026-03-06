#include <stdint.h>
#include <stddef.h>

#include "tty.h"
#include "kstd.h"
#include "vga.h"
volatile tty ttys[20];

void save_terminal_state(){
    memcpy(ttys[active_tty].inputbuf,inputbuf,128);
    memcpy(ttys[active_tty].mem,(void*)0xB8000,4000);
    ttys[active_tty].terminal_row = vga_get_row();
    ttys[active_tty].terminal_col = vga_get_col();
}

void load_terminal_state(){
    memcpy((void*)0xB8000,ttys[active_tty].mem,4000);
    memcpy(inputbuf,ttys[active_tty].inputbuf,128);

    vga_move_cursor(0,0);
    vga_set_col(0);
    vga_set_row(0);

    terminal_writestring("Currently in tty: "); char buf[4]; itoa(active_tty,buf,10); terminal_writestring(buf);
    if(ttys[active_tty].terminal_row == 0){
        vga_move_cursor(ttys[active_tty].terminal_row+1,ttys[active_tty].terminal_col);
        vga_set_row(ttys[active_tty].terminal_row+1);

    }
    else{
        vga_move_cursor(ttys[active_tty].terminal_row,ttys[active_tty].terminal_col);

        vga_set_row(ttys[active_tty].terminal_row);
    }
    vga_set_col(ttys[active_tty].terminal_col);
    terminal_writestring(" ");
    terminal_removechar();


}


