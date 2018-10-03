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
#include "lib/commandlinereader.h"
#include "lib/vector.h"
#include "finprocess.h"

unsigned int MAXCHILDREN = UINT_MAX;

void displayUsage(const char* appName) {
    printf("Usage: %s\n", appName);
    printf("Additional Argument:                              (default)\n");
    printf("    MAXCHILDREN <ULONG>   child processes limit   ULONG_MAX\n");
    printf("\nShell:\n");
    printf("    run <inputfile>       runs CircuitRouter-SeqSolver with <inputfile>\n");
    printf("    exit                  exits the console\n");
}

void parseArgs(int argc, char** argv) {
    if (argc > 2) {
        displayUsage(argv[0]);
        exit(1);
    }
    if (argc == 2) {
        MAXCHILDREN = strtoul(argv[1], NULL, 10);    
        if (MAXCHILDREN == 0) {
            fprintf(stderr, "Invalid MAXCHILDREN\n");
            displayUsage(argv[0]);
            exit(1);
        }
    }
}

#define ARGVECTORSIZE 4
#define BUFFERSIZE 128

int command(const char* command, char** argVector) {
    return strcmp(argVector[0], command) == 0;
}

int length(char **v, int vSize) {
    int i;
    for (i = 0; i < vSize; i++) {
        if (v[i] == NULL) {
            break;
        }
    }
    return i;
}

int main(int argc, char** argv) {
    parseArgs(argc, argv);
    char *argVector[ARGVECTORSIZE];
    char buffer[BUFFERSIZE];

    int nChilds = 0;
    vector_t *forks = vector_alloc(1);
    pid_t pid;
    int status;

    while (1) {
        //printf("> ");
        readLineArguments(argVector, ARGVECTORSIZE, buffer, BUFFERSIZE);

        if (command("run", argVector)) {
            while (nChilds >= MAXCHILDREN) {
                pid = wait(&status);
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
                if (length(argVector, ARGVECTORSIZE) != 2) {
                    fprintf(stderr, "run must receive (only) <inputfile>\n");
                    displayUsage(argv[0]);
                }
                char* execArgs[] =  {"CircuitRouter-SeqSolver", argVector[1], NULL};
                status = execv("CircuitRouter-SeqSolver/CircuitRouter-SeqSolver", execArgs);
                printf("%i\n", status);
                exit(status);
            } else {
                nChilds++;
            }
        } else if (command("exit", argVector)) {
            while (nChilds > 0) {
                pid = wait(&status);
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


