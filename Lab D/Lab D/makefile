all: multi
multi:
	nasm -g -f elf32 -o multi.o multi.s
	gcc -m32 -g -Wall -o multi multi.o
	rm -f *.o

.PHONY: clean
clean:
	rm -f *.o multi