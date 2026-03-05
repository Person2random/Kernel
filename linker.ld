/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */

ENTRY(_start)

SECTIONS
{
    /* Load kernel at 2MB */
    . = 2M;
	_kernel_start = .;
    /* First loadable section */
    .text ALIGN(4K) :
    {
        /* Multiboot header must be first thing in first loadable segment */
        KEEP(*(.multiboot))
        *(.text*)
    }

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }

    .data ALIGN(4K) :
    {
        *(.data*)
    }

    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss*)
    }

    _kernel_end = .;
}