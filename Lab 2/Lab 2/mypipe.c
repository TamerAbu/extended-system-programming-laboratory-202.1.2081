#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv){
    int pipefd[2];
    pipe(pipefd);
    if(fork() == 0){
        close(pipefd[0]);
        write(pipefd[1], "Hello\n", 6);
        close(pipefd[1]);
    } else {
        char buffer[6];
        close(pipefd[1]);
        read(pipefd[0], buffer, 6);
        close(pipefd[0]);
        printf("Received %s", buffer);
    }

}