.intel_syntax noprefix
.global irq0
.global irq1

irq0:              # Timer IRQ
    pusha
    call irq0_handler   # the linker will resolve this
    popa
    mov al, 0x20
    out 0x20, al        # EOI to master PIC
    iret

irq1:              # Keyboard IRQ
    pusha
    call irq1_handler
    popa
    mov al, 0x20
    out 0x20, al        # EOI to master PIC
    iret
