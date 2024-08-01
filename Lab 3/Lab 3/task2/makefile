all: task2

task2: util.o
	nasm -g -f elf32 start.s -o start.o
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o
	ld -g -m elf_i386 start.o main.o util.o -o main
	rm -f *.o

util.o:
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

.PHONY: clean
clean:
	rm -f *.o task2