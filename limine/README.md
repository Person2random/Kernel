# Limine vendor README

This directory contains vendored Limine BIOS artifacts used to build an ISO that boots with Limine in BIOS/legacy (CSM) mode.

Files:
- limine-cd.bin   (El Torito BIOS boot image — copied into the ISO as boot/limine/limine-cd.bin)
- limine.sys      (isohybrid MBR used by xorriso - placed in the repo and referenced during ISO creation)

Note: Due to repository size considerations, these files were initially placeholders. You can populate them automatically by running the provided install script from the repository root:

  ./limine/install-limine.sh

This script downloads the official Limine binary bundle for v12.3.3 and extracts the required BIOS artifacts into this directory.

If you prefer to vendor the files manually, download limine-binary.zip from the Limine release page (https://github.com/Limine-Bootloader/Limine/releases/tag/v12.3.3) and copy the files into this directory.
