# Compiles OS onto image diskc.img

dd if=/dev/zero of=diskc.img bs=512 count=256
nasm bootload.asm
dd if=bootload of=diskc.img conv=notrunc

bcc -ansi -c -o kernel_c.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o

bcc -ansi -c -o shell_c.o shell.c
as86 userlib.asm -o userlib_asm.o
ld86 -o shell -d shell_c.o userlib_asm.o

gcc -o loadFile loadFile.c

bcc -ansi -c -o letter.o letter.c
ld86 -o letter -d letter.o userlib_asm.o
bcc -ansi -c -o number.o number.c
ld86 -o number -d number.o number.c

./loadFile kernel
./loadFile shell
./loadFile letter
./loadFile number
