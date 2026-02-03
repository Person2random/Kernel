default: 
	make clean
	make build
	make run
	make clean

build:
	i686-elf-as boot.s -o boot.o
	i686-elf-as -32 gdtflush.s -o gdtflush.o
	i686-elf-as -32 idtload.s -o loadidt.o
	i686-elf-as -32 isr.s -o isr0.o
	i686-elf-as -32 irq.s -o irq.o

	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c vga.c -o terminal.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c gdt.c -o gdt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c isr.c -o isr.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c idt.c -o idt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c kstd.c -o kstd.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

	i686-elf-gcc -T linker.ld -o myos -ffreestanding -O2 -nostdlib \
		boot.o kernel.o terminal.o gdt.o gdtflush.o loadidt.o isr0.o isr.o idt.o irq.o kstd.o -lgcc

	mkdir -p isodir/boot/grub
	cp myos isodir/boot/myos
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir

run:
	qemu-system-i386 -kernel myos
clean:
	rm -rf boot.o isodir kernel.o myos myos.iso	terminal.o gdt.o gdtflush.o loadidt.o isr0.o idt.o isr.o irq.o kstd.o
