/*
* CircuitRouter-SimpleShell
* João David, 89471
* Gonçalo Almeida, 89448
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <string.h>
#include "../lib/commandlinereader.h"
#include "../lib/vector.h"
#include "../customlib/finprocess.h"

unsigned long MAXCHILDREN = ULONG_MAX;

void displayUsage(const char* appName) {
    printf("Usage: %s\n", appName);
    puts("Additional Argument:                              (default)");
    puts("    MAXCHILDREN <ULONG>   child processes limit   ULONG_MAX");
    exit(1);
}

void parseArgs(int argc, char** argv) {
    if (argc > 2) {
        displayUsage(argv[0]);
    }
    if (argc == 2) {
        MAXCHILDREN = strtoul(argv[1], NULL, 10);    
        if (MAXCHILDREN == 0) {
            fprintf(stderr, "Invalid MAXCHILDREN\n");
            displayUsage(argv[0]);
        }
    }
}

#define ARGVECTORSIZE 10
#define BUFFERSIZE 128

int command(const char* command, char** argVector) { //TODO meter argVector global?
    return strcmp(argVector[0], command) == 0;
}

int main(int argc, char** argv) {
    parseArgs(argc, argv);
    char *argVector[ARGVECTORSIZE];
    char buffer[BUFFERSIZE];

    int nChilds = 0;
    vector_t *forks = vector_alloc(1); //TODO tamanho inicial numa variavel
    pid_t pid;
    int status;

    while (1) {
        printf("> ");
        readLineArguments(argVector, ARGVECTORSIZE, buffer, BUFFERSIZE);
        //for (int i = 0; i < ARGVECTORSIZE; i++) puts(argVector[i]);
        if (command("run", argVector)) {
            while (nChilds >= MAXCHILDREN) {
                pid = wait(&status);
                printf("Child %i exited with status %i\n", pid, status);
                Process proc = process_alloc(pid, status);
                vector_pushBack(forks, proc);
                nChilds--;
            }
            pid_t pid = fork(); 
            if (pid == -1) {
                fprintf(stderr, "Error creating child process.\n");
                exit(1);
            }
            if (pid == 0) {
                status = execl("./CircuitRouter-SeqSolver", argVector[1]);
                printf("I'm child  %i and I finished execl with status %i\n", getpid(), status);
                exit(status);
            } else {
                printf("My child is %i\n", pid);
                nChilds++;
            }
        } else if (command("exit", argVector)) {
            while (nChilds > 0) {
                pid = wait(&status);
                printf("Child %i exited with status %i\n", pid, status);
                Process proc = process_alloc(pid, status);
                vector_pushBack(forks, proc);
                nChilds--;
            }
            break; 
        } else {
            fprintf(stderr, "Invalid command\n");
        }
    }

    return 0;
}
