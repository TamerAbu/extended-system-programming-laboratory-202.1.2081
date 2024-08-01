all: task1 task4
task1:
	gcc -Wall -g -m32 task1.c -o task1

task4: 
	gcc -fno-pie -fno-stack-protector -m32 task4.c -o task4

.PHONY: clean
	
clean:
	rm -f task1 task4
