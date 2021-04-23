rm ./output/system.img

dd if=/dev/zero of=./output/system.img bs=512 count=2880
nasm ./asm/bootloader.asm -o ./output/bootloader
dd if=./output/bootloader of=./output/system.img bs=512 count=1 conv=notrunc

bcc -ansi -c -o ./output/kernel.o ./src/kernel.c
bcc -ansi -c -o ./output/shell.o ./src/shell/shell.c
bcc -ansi -c -o ./output/utils.o ./src/utils/utils.c
bcc -ansi -c -o ./output/string.o ./src/string/string.c
bcc -ansi -c -o ./output/sector.o ./src/sector/sector.c
bcc -ansi -c -o ./output/math.o ./src/math/math.c
bcc -ansi -c -o ./output/folderIO.o ./src/folderIO/folderIO.c
bcc -ansi -c -o ./output/fileIO.o ./src/fileIO/fileIO.c

nasm -f as86 ./asm/lib.asm -o ./output/lib_asm.o

bcc -ansi -c -o ./output/ls.o ./src/lib/ls.c
bcc -ansi -c -o ./output/cat.o ./src/lib/cat.c
bcc -ansi -c -o ./output/ln.o ./src/lib/ln.c
bcc -ansi -c -o ./output/cp.o ./src/lib/cp.c
bcc -ansi -c -o ./output/mkdir.o ./src/lib/mkdir.c
bcc -ansi -c -o ./output/mv.o ./src/lib/mv.c
bcc -ansi -c -o ./output/rm.o ./src/lib/rm.c

bcc -ansi -c -o ./output/shell.o ./src/shell/shell.c

ld86 -o ./bin/ls -d ./output/ls.o ./output/lib_asm.o ./output/utils.o ./output/string.o ./output/sector.o ./output/math.o ./output/folderIO.o ./output/fileIO.o
ld86 -o ./bin/cat -d ./output/cat.o ./output/lib_asm.o ./output/utils.o ./output/string.o ./output/sector.o ./output/math.o ./output/folderIO.o ./output/fileIO.o
ld86 -o ./bin/ln -d ./output/ln.o ./output/lib_asm.o ./output/utils.o ./output/string.o ./output/sector.o ./output/math.o ./output/folderIO.o ./output/fileIO.o
ld86 -o ./bin/cp -d ./output/cp.o ./output/lib_asm.o ./output/utils.o ./output/string.o ./output/sector.o ./output/math.o ./output/folderIO.o ./output/fileIO.o
ld86 -o ./bin/mkdir -d ./output/mkdir.o ./output/lib_asm.o ./output/utils.o ./output/string.o ./output/sector.o ./output/math.o ./output/folderIO.o ./output/fileIO.o
ld86 -o ./bin/mv -d ./output/mv.o ./output/lib_asm.o ./output/utils.o ./output/string.o ./output/sector.o ./output/math.o ./output/folderIO.o ./output/fileIO.o
ld86 -o ./bin/rm -d ./output/rm.o ./output/lib_asm.o ./output/utils.o ./output/string.o ./output/sector.o ./output/math.o ./output/folderIO.o ./output/fileIO.o

ld86 -o ./bin/shell -d ./output/shell.o ./output/lib_asm.o ./output/utils.o ./output/string.o ./output/sector.o ./output/math.o ./output/folderIO.o ./output/fileIO.o

nasm -f as86 ./asm/kernel.asm -o ./output/kernel_asm.o
ld86 -o ./output/kernel -d ./output/kernel.o ./output/kernel_asm.o ./output/utils.o ./output/string.o ./output/sector.o ./output/math.o ./output/folderIO.o ./output/fileIO.o


dd if=/dev/zero of=./output/map.img bs=512 count=1
dd if=/dev/zero of=./output/files.img bs=512 count=2
dd if=/dev/zero of=./output/sectors.img bs=512 count=1

# echo fill map img and loadfile
python3 ./script/map.py

# echo linking to system.img
dd if=./output/kernel of=./output/system.img bs=512 conv=notrunc seek=1
dd if=./output/map.img of=./output/system.img bs=512 count=1 seek=256 conv=notrunc
dd if=./output/files.img of=./output/system.img bs=512 count=2 seek=257 conv=notrunc
dd if=./output/sectors.img of=./output/system.img bs=512 count=1 seek=259 conv=notrunc

python3 ./script/loadfile.py ./data/iseng.txt
python3 ./script/loadProgram.py ./bin/ls ./bin/shell ./bin/cat ./bin/ln ./bin/cp ./bin/mkdir ./bin/mv ./bin/rm

# # echo start bochs
bochs -f if2230.config