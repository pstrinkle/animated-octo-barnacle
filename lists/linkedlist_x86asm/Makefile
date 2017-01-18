# Basic Makefile, god knows these can get fun and complicated with chains of 
# implicit rules.

# This was only built and tested on an x86_64 darwin system.

#ld -o src/fun.obj src/fun.o -arch i386
# x86_64 for ld

all:
	nasm -f macho -o src/linkedlist.o src/linkedlist.asm
	gcc -m32 -o linkedlist src/linkedlist.o src/test.c

clean:
	rm -f src/*.o src/*.obj
