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

void displayUsage(const char* appName) { //TODO maybe change conflicting names
    printf("Usage: %s\n", appName);
    puts("Additional Argument:                              (default)");
    puts("    MAXCHILDREN <ULONG>   child processes limit   ULONG_MAX");
    exit(1);
}

void parseArgs(int argc, char** argv) { //TODO same as displayUsage
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

#define ARGVECTORSIZE 10
#define BUFFERSIZE 128

int command(char** argVector, const char * command) { //TODO meter argVector global?
    return strcmp(argVector, command) == 0;
}

int main(int argc, char** argv) {
    parseArgs(argc, argv);
    char *argVector[ARGVECTORSIZE];
    char buffer[BUFFERSIZE];

    int nChilds = 0;
    int childPids[MAXCHILDREN]; //TODO faz sentido usar maxchildren mas ao mesmo tempo pode ocupar demasiado espaço, caso não seja especificado

    while (1) {
        printf("> ");
        readLineArguments(argVector, ARGVECTORSIZE, buffer, BUFFERSIZE);
        for (int i = 0; i < ARGVECTORSIZE; i++) puts(argVector[i]);
        if (command("run")) {
            
        }
    }

    return 0;
}
