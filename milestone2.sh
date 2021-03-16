nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc
bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o shell.o shell.c
bcc -ansi -c -o utils.o utils.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o shell.o utils.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=1
bochs -f if2230.config

# echo Disk, Map, Sector, and Files Image
# dd if=/dev/zero of=system.img bs=512 count=2880
# dd if=/dev/zero of=map.img bs=512 count=1
# dd if=/dev/zero of=files.img bs=512 count=2
# dd if=/dev/zero of=sectors.img bs=512 count=1
 
# echo Bootloader
# nasm bootloader.asm -o bootloader
 
# echo Bootloader to Disk Image
# dd if=bootloader of=system.img bs=512 count=1 conv=notrunc
 
# echo Compile Kernel to Object Code with BCC
# bcc -ansi -c -o kernel.o kernel.c
# bcc -ansi -c -o shell.o shell.c
# bcc -ansi -c -o utils.o utils.c
 
# echo Compile ASM Kernel to Object Code with NASM
# nasm -f as86 kernel.asm -o kernel_asm.o
 
# echo Link Object Code with ld86
# ld86 -o kernel -d kernel.o kernel_asm.o shell.o utils.o
 
# echo Kernel to Disk Image in Sector 1
# dd if=kernel of=system.img bs=512 conv=notrunc seek=1