#include <stdio.h>
#include <linux/limits.h>
#include <stdlib.h>
#include "LineParser.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <wait.h>

int debugMode = 0;

void execute(cmdLine *pCmdLine){
    int pid = fork();
    if(pid == 0){ //child 
        // if debug mode is on print
        if(debugMode){
            fprintf(stdout,"PID: %d\n", getpid());
            fprintf(stdout,"Command: %s\n", pCmdLine->arguments[0]);
        }

        if(pCmdLine->inputRedirect){
            fclose(stdin);
            if(fopen(pCmdLine->inputRedirect, "r") == NULL){
                perror("fopen");
                _exit(EXIT_FAILURE);
            }
        }
        if(pCmdLine->outputRedirect){
            fclose(stdout);
            if(fopen(pCmdLine->outputRedirect, "w") == NULL){
                perror("fopen");
                _exit(EXIT_FAILURE);
            }
        }
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror("execvp");
        _exit(EXIT_FAILURE);
    }
    if(pCmdLine->blocking){
        waitpid(pid, NULL, 0);
    }
}
void handle_commands(cmdLine* pCmdLine){
    if(strcmp(pCmdLine->arguments[0], "quit") == 0){
        freeCmdLines(pCmdLine);
        exit(EXIT_SUCCESS);
    }
    if(strcmp(pCmdLine->arguments[0], "wake") == 0){
        if(kill(atoi(pCmdLine->arguments[1]), SIGCONT) < 0){
            perror("kill");
        }
    }
    else if(strcmp(pCmdLine->arguments[0], "suspend") == 0){
        if(kill(atoi(pCmdLine->arguments[1]), SIGTSTP) < 0){
            perror("kill");
        }
    }
    else if(strcmp(pCmdLine->arguments[0], "kill") == 0){
        if(kill(atoi(pCmdLine->arguments[1]), SIGINT) < 0){
            perror("kill");
        }
    }else {
        execute(pCmdLine);
    }
    freeCmdLines(pCmdLine);
}


int main(int argc, char** argv){
    char cwd[PATH_MAX];
    char input[2048];
    getcwd(cwd, PATH_MAX);

    //check for debug mode
    for (int i = 1; i < argc; i++){
        if (strcmp (argv[i], "-d") == 0){
            debugMode = 1;
        } else if(debugMode){
            fprintf (stderr, "%s\n", argv[i]);
        }
    }

    while(1){
        printf("%s$ ", cwd);
        if(fgets(input, 2048, stdin) == NULL){
            printf("\n");
            break;
        }
        cmdLine* cmd = parseCmdLines(input);
        
        if(strcmp(cmd->arguments[0], "cd") == 0){
            if(chdir(cmd->arguments[1]) < 0){
                perror("chdir");
            } else {
                getcwd(cwd, PATH_MAX);
            }
        } else {
            handle_commands(cmd);
        }
    }   
}