all: encoder

encoder: encoder.c
	gcc -g -Wall -m32 -o encoder encoder.c

.PHONY: clean
clean:
	rm -f *.o encoder