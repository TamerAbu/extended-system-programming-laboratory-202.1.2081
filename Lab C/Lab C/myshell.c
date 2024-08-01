#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include "LineParser.h"
#include <linux/limits.h>

#define RUNNING 1
#define TERMINATED -1
#define SUSPENDED 0
#define HISTLEN 5

typedef struct process{
    cmdLine* cmd;
    pid_t pid;
    int status;
    struct process *next;
} process;

int debugMode = 0;
process* myList = NULL;
char history[HISTLEN][2048];
int newest = -1;
int oldest = 0;
int histSize;

int calcStatus (int status){
    if(WIFEXITED(status) || WIFSIGNALED(status))
        return TERMINATED;
    if(WIFSTOPPED(status))
        return SUSPENDED;
    return RUNNING;
}
void freeProcessList (process* process_list){
    if(process_list){
        freeCmdLines(process_list->cmd);
        free(process_list);
    }
}


void addProcess (process** process_list, cmdLine* cmd, pid_t pid){
    process* p = malloc(sizeof(struct process));
    p->cmd = cmd;
    p->pid = pid;
    p->next = process_list[0];
    process_list[0] = p;
    if(p->cmd->next + p->cmd->blocking > 0)
        p->status = TERMINATED;
    else 
        p->status = RUNNING;
}

void updateProcessStatus (process* process_list, int pid, int status){
    while (process_list){
        if(process_list->pid == pid){
            process_list->status = calcStatus(status);
        }
        process_list = process_list->next;
    }
}

void updateProcessList(process **process_list){
    int pid, status;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0){
        updateProcessStatus(myList, pid, status);
    }
}

void removeFreshlyTerminated (){
    process* temp = myList;
    process* prev = NULL;
    while(temp != NULL){
        if(temp->status == TERMINATED){
            if(prev == NULL){
                myList = myList->next;
                freeProcessList(temp);
                temp = myList;
            } else {
                prev->next = temp->next;
                freeProcessList(temp);
                temp = prev->next;
            }
        } else {
            prev = temp;
            temp = temp->next;
        }
    }
}

void printProcessList (process** process_list){
    updateProcessList (&myList);
    printf("index\t\tPID\t\tCommand\t\tSTATUS\n");
    process* p = process_list[0];
    int counter = 0;
    while(p != NULL){
        char* status = "TERMINATED";
        if(p->status == RUNNING) {
            status = "RUNNING";
        }
        if(p->status == SUSPENDED) {
            status = "SUSPENDED";
        }
        printf("%d\t\t%d\t\t%s\t\t%s\n", counter, p->pid, p->cmd->arguments[0], status);
        p = p->next;
        counter++;
    }
    removeFreshlyTerminated();
}

void handle_pipe (cmdLine* cmd1, cmdLine* cmd2){
    if (cmd1->outputRedirect){
        fprintf(stderr, "invalid output redirect in cmd1\n");
        return;
    }
    if (cmd2->inputRedirect){
        fprintf(stderr, "invalid input redirect in cmd2\n");
        return;
    }
    int pipefd[2];
    if (pipe(pipefd) < 0){
        perror("pipe");
        return;
    }
    if (debugMode) fprintf(stderr, "(parent_process>forking...)\n");
    int pid1 = fork();
    if (pid1 == 0){
        if(cmd1->inputRedirect){
            fclose(stdin);
            if(fopen(cmd1->inputRedirect, "r") == NULL){
                perror("fopen");
                _exit(EXIT_FAILURE);
            }
        }
        if(debugMode) {
            fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");
        }
        close(STDOUT_FILENO);
        dup(pipefd[1]);
        close(pipefd[1]);
        char* const* arg = cmd1->arguments;
        if(debugMode) {
            fprintf(stderr, "(child1>going to execute cmd: %s)\n", arg[0]);
        }
        execvp(arg[0], arg);
        perror("execvp");
        _exit(EXIT_FAILURE);
    } 
    if(debugMode) {
        fprintf(stderr, "(parent_process>created process with id: %d)\n", pid1);
    }
    if(debugMode) {
        fprintf(stderr, "(parent_process>closing the write end of the pipe...)\n");
    }
    close(pipefd[1]);
    if(debugMode) {
        fprintf(stderr, "(parent_process>forking...)\n");
    }
    int pid2 = fork();
    if(pid2 == 0){
        if(cmd2->outputRedirect){
            fclose(stdout);
            if(fopen(cmd2->outputRedirect, "w") == NULL){
                perror("fopen");
                _exit(EXIT_FAILURE);
            }
        }
        if(debugMode) {
            fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe...)\n");
        }
        close(STDIN_FILENO);
        dup(pipefd[0]);
        close(pipefd[0]);
        char* const* arg = cmd2->arguments;
        if(debugMode) {
            fprintf(stderr, "(child2>going to execute cmd: %s)\n", arg[0]);
        }
        execvp(arg[0], arg);
        perror("execvp");
        _exit(EXIT_FAILURE);
    }
    if(debugMode) {
        fprintf(stderr, "(parent_process>created process with id: %d)\n", pid2);
    }
    if(debugMode) { 
        fprintf(stderr, "(parent_process>closing the read end of the pipe...)\n"); 
    }
    close(pipefd[0]);
    if(debugMode) {
        fprintf(stderr, "(parent_process>waiting for child processes to terminate...)\n");
    }
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    if(debugMode) {
        fprintf(stderr, "(parent_process>exiting...)\n");
    }
    addProcess(&myList, cmd1, pid1);
}

void execute(cmdLine *pCmdLine){
    if(pCmdLine->next){
        handle_pipe(pCmdLine, pCmdLine->next);
    } else {
        int pid = fork();
        if(pid == 0){
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
        addProcess(&myList, pCmdLine, pid);
    }
}

void addToHistory(char* input){
    printf("adding to history: %s", input);
    newest = (newest + 1) % HISTLEN;
    strcpy(history[newest], input);
    if(histSize == HISTLEN){
        oldest = (oldest + 1) % HISTLEN;
    } else {
        histSize++;
    }
}

void handle_commands(cmdLine* pCmdLine, char* input){
    if(strcmp(pCmdLine->arguments[0], "history") == 0){
        for(int i = 0; i < histSize; i++){
            printf("%d) %s", i + 1, history[(oldest + i) % HISTLEN]);
        }
        freeCmdLines(pCmdLine);
        return;
    } else if(strcmp(pCmdLine->arguments[0], "!!") == 0){
        freeCmdLines(pCmdLine);
        if(histSize == 0){
            fprintf(stderr, "History is empty!\n");
            return;
        }
        pCmdLine = parseCmdLines(history[newest]);
    } else if(strncmp(pCmdLine->arguments[0], "!", 1) == 0){
        int index = atoi(pCmdLine->arguments[0]+1) - 1;
        freeCmdLines(pCmdLine);
        if(index < 0 || index >= histSize){
            fprintf(stderr, "Index %d out of bounds %d!\n", index, histSize);
            return;
        }
        pCmdLine = parseCmdLines(history[(oldest + index) % HISTLEN]);
    } else {
        addToHistory(input);
    }
    if(strcmp(pCmdLine->arguments[0], "quit") == 0){
        freeCmdLines(pCmdLine);
        while(myList){
            process* next = myList->next;
            freeProcessList(myList);
            myList = next;
        }
        exit(EXIT_SUCCESS);
    }
    if(strcmp(pCmdLine->arguments[0], "wake") == 0){
        if(kill(atoi(pCmdLine->arguments[1]), SIGCONT) < 0){
            perror("kill");
        }
        freeCmdLines(pCmdLine);
    }
    else if(strcmp(pCmdLine->arguments[0], "suspend") == 0){
        if(kill(atoi(pCmdLine->arguments[1]), SIGTSTP) < 0){
            perror("kill");
        }
        freeCmdLines(pCmdLine);
    }
    else if(strcmp(pCmdLine->arguments[0], "kill") == 0){
        if(kill(atoi(pCmdLine->arguments[1]), SIGINT) < 0){
            perror("kill");
        }
        freeCmdLines(pCmdLine);
    } else if(strcmp(pCmdLine->arguments[0], "procs") == 0){
        printProcessList(&myList);
        freeCmdLines(pCmdLine);
    }else {
        execute(pCmdLine);
    }
}


int main(int argc, char** argv){
    char cwd[PATH_MAX];
    char input[2048];
    getcwd(cwd, PATH_MAX);
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
            while(myList){
                process* next = myList->next;
                freeProcessList(myList);
                myList = next;
            }
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
            handle_commands(cmd, input);
        }
    }   
}