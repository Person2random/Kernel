default: clean build run clean

build:
	i686-elf-as boot.s -o boot.o
	i686-elf-as -32 gdtflush.s -o gdtflush.o
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c vga.c -o terminal.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c gdt.c -o gdt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -T linker.ld -o myos -ffreestanding -O2 -nostdlib \
	boot.o kernel.o terminal.o gdt.o gdtflush.o -lgcc
	mkdir -p isodir/boot/grub
	cp myos isodir/boot/myos
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir

run:
	qemu-system-i386 -kernel myos
clean:
	rm -rf boot.o isodir kernel.o myos myos.iso	terminal.o gdt.o gdtflush.o
