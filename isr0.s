.intel_syntax noprefix
.section .text
.global isr0
isr0:
    cli
    hlt