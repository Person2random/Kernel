#include <stdint.h>
#ifndef TTY_H
#define TTY_H

typedef void (*in_cb_t)(char *buf);

typedef struct {
	uint8_t mem[4000];
	in_cb_t cb;
	char inputbuf[128];
	uint16_t terminal_row;
	uint16_t terminal_col;
}tty;
extern volatile tty ttys[20];
void init_tty();
extern uint8_t active_tty;
void save_terminal_state();
void load_terminal_state();
#endif
