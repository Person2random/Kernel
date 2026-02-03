#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Initialize IDT entries 0–31
void isr_init(void);

// Called by all ISR stubs
void isr_handler(int num);
#endif
