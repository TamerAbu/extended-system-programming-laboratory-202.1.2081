#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char** argv){
    int pipefd[2];

    if(pipe(pipefd) < 0){

        perror("pipe");
        return 1;
        
    }

    fprintf (stderr, "(parent_process>forking...)\n");
    int child1 = fork();
    
    if (child1 == 0){

        fprintf (stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");
        close (STDOUT_FILENO);
        dup (pipefd[1]);
        close (pipefd[1]);
        char* arg[] = {"ls", "-l", NULL};
        fprintf (stderr, "(child1>going to execute cmd: %s)\n", arg[0]);
        execvp (arg[0], arg);
        perror ("execvp");
        _exit (EXIT_FAILURE);

    } 

    fprintf (stderr, "(parent_process>created process with id: %d)\n", child1);
    fprintf (stderr, "(parent_process>closing the write end of the pipe...)\n");
    close (pipefd[1]);
    fprintf (stderr, "(parent_process>forking...)\n");
    int child2 = fork();

    if (child2 == 0){

        fprintf (stderr, "(child2>redirecting stdin to the read end of the pipe...)\n");
        close (STDIN_FILENO);
        dup (pipefd[0]);
        close (pipefd[0]);
        char* arg[] = {"tail", "-n", "2", NULL};
        fprintf (stderr, "(child2>going to execute cmd: %s)\n", arg[0]);
        execvp (arg[0], arg);
        perror ("execvp");
        _exit (EXIT_FAILURE);

    }

    fprintf (stderr, "(parent_process>created process with id: %d)\n", child2);
    fprintf (stderr, "(parent_process>closing the read end of the pipe...)\n");
    close (pipefd[0]);
    fprintf (stderr, "(parent_process>waiting for child processes to terminate...)\n");
    waitpid (child1, NULL, 0);
    waitpid (child2, NULL, 0);
    fprintf (stderr, "(parent_process>exiting...)\n");
}
