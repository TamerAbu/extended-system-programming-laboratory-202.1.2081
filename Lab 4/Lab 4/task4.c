#include <stdlib.h>
#include <stdio.h>

int counter(char* input){
    int counter = 0;
    while(*input != '\0'){
        if(*input >= '0' && *input <= '9'){
            counter++;
        }
        input++;
    }
    return counter;
}

int main(int argc, char** argv){
    if(argc == 2)
        printf("Count = %d\n", counter(argv[1]));
}