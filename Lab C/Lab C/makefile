all: myshell mypipeline
myshell:
	gcc -c -m32 -g -Wall -o LineParser.o LineParser.c
	gcc -c -m32 -g -Wall -o myshell.o myshell.c
	gcc -m32 -g -Wall -o myshell myshell.o LineParser.o
	rm -f *.o

looper:
	gcc -m32 -g -Wall -o looper looper.c

mypipeline:
	gcc -m32 -g -Wall -o mypipeline mypipeline.c

.PHONY: clean
clean:
	rm -f *.o myshell mypipeline looper