.intel_syntax noprefix
.global gdt_flush
.extern kernel_stack   # declare the symbol for the linker
.section .text

gdt_flush:
    cli                 # disable interrupts

    mov eax, [esp + 4]  # get pointer to gp (argument passed on stack)
    lgdt [eax]          # load pointer to GDT

    mov ax, 0x10        # data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    # Correctly reload stack segment
    mov ss, ax
    mov ebx, [esp]               # save return address (was on old stack)
    lea esp, [kernel_stack + 16384]  # point ESP to top of new stack
    push ebx                    # restore return address onto new stack

    # Far jump to reload CS
    jmp 0x08:flush_cs
flush_cs:
    ret