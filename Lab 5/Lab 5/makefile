all: task2

task2:
	nasm -f elf32 -o start.o start.s
	gcc -m32 -c loader.c -o loader.o
	ld -o loader loader.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

.PHONY: clean
clean:
	rm -f loader