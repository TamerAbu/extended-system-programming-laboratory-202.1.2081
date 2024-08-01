all: task1

task1: util.o
	nasm -f elf32 task1.s -o task1.o
	gcc -m32 util.o task1.o -o task1
	rm -f *.o

util.o:
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

.PHONY: clean
clean:
	rm -f *.o task1