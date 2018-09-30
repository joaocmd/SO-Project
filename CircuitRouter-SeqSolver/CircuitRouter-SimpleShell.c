/*
* CircuitRouter-SimpleShell
* João David, 89471
* Gonçalo Almeida, 89448
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "../lib/commandlinereader.h"

unsigned long MAXCHILDREN = ULONG_MAX;

void simpleshell_displayUsage(const char* appName) { //TODO try simpler function names
    printf("Usage: %s\n", appName);
    puts("Additional Argument:                              (default)");
    puts("    MAXCHILDREN <ULONG>   child processes limit   ULONG_MAX");
    exit(1);
}

void simpleshell_parseArgs(int argc, char** argv) { //TODO same as displayUsage
    if (argc > 2) {
        simpleshell_displayUsage(argv[0]);
    }
    if (argc == 2) {
        MAXCHILDREN = strtoul(argv[1], NULL, 10);    
        if (MAXCHILDREN == 0) {
            fprintf(stderr, "Invalid MAXCHILDREN\n");
            simpleshell_displayUsage(argv[0]);
        }
    }
}

int main(int argc, char** argv) {

    simpleshell_parseArgs(argc, argv);

    int pid;
    pid = fork();

    if (pid == -1) {
        fprintf(stderr, "Error creating child process\n");
        exit(1);
    }
    
    if (pid == 0) {
        printf("I'm child process with pid = %i\n", getpid());
    } else {
        printf("I'm parent process with pid = %i, my child is %i\n", getpid(), pid);
    }


    return 0;
}
