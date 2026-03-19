#include <stdint.h>
#ifndef TTY_H
#define TTY_H

typedef void (*in_cb_t)(char *buf);

typedef struct {
	uint16_t mem[2000];
	in_cb_t cb;
	char inputbuf[128];
	uint16_t terminal_row;
	uint16_t terminal_col;
	uint8_t terminal_color;
}tty;
extern volatile tty ttys[20];
void init_tty();
extern uint8_t active_tty;
void save_terminal();
void show_terminal();
void append_ibuf(uint8_t sc);
void ttyprint(const char* s, int tty);
void ttys_initialize(void);
#endif
