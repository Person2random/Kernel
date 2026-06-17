# Kernel repo README

This branch migrates ISO boot from GRUB to Limine (BIOS/legacy) with minimal changes to the kernel.

What changed
- Add limine.cfg at repository root to direct Limine to /boot/myos
- Add limine/ directory and helper script to fetch Limine v12.3.3 BIOS artifacts
- Update Makefile to produce an ISO that boots with Limine instead of grub-mkrescue

How to build and test (local)
1. Populate Limine artifacts (if not already present):
   ./limine/install-limine.sh

2. Build the kernel and ISO:
   make build

3. Run in QEMU:
   make run

Notes
- This migration keeps your existing i686 Multiboot kernel unchanged; it only replaces GRUB with Limine for ISO/BIOS boot.
- If you want the Limine binaries fully vendored without fetching at build time, you can manually download limine-binary.zip from the Limine release page (https://github.com/Limine-Bootloader/Limine/releases/tag/v12.3.3) and copy limine-cd.bin and limine.sys into limine/.
