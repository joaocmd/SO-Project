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
#define CH_APPNAME "CircuitRouter"
#define CH_APPPATH "CircuitRouter-SeqSolver/CircuitRouter-SeqSolver"
#define ARGVECTORSIZE 4
#define BUFFERSIZE 128

void displayUsage(const char* appName) {
    printf("Usage: %s\n", appName);
    printf("Additional Argument:                              (default)\n");
    printf("    MAXCHILDREN <ULONG>   child processes limit   ULONG_MAX\n");
    printf("Shell:\n");
    printf("    run <inputfile>       runs %s with <inputfile>\n", CH_APPNAME);
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
            fprintf(stderr, "Invalid MAXCHILDREN.\n");
            displayUsage(argv[0]);
            exit(1);
        }
    }
}

int command(const char* command, char** argVector) {
    return strcmp(argVector[0], command) == 0;
}

int length(char **v, int vCapacity) { //TODO ficheiro diferente
    int i;
    for (i = 0; i < vCapacity; i++) {
        if (v[i] == NULL) {
            break;
        }
    }
    return i;
}

void waitAndSave(vector_t *forks, int *nChildren) {
    pid_t pid;
    int status;
    pid = wait(&status);
    Process proc = process_alloc(pid, status);
    vector_pushBack(forks, proc);
    (*nChildren)--;
}

int main(int argc, char** argv) {
    parseArgs(argc, argv);
    char *argVector[ARGVECTORSIZE];
    char buffer[BUFFERSIZE];

    int nChildren = 0;
    vector_t *forks = vector_alloc(4);

    while (1) {
        readLineArguments(argVector, ARGVECTORSIZE, buffer, BUFFERSIZE);

        if (command("run", argVector)) {
            while (nChildren >= MAXCHILDREN) {
                waitAndSave(forks, &nChildren);
            }
            pid_t pid = fork(); 
            if (pid == -1) {
                fprintf(stderr, "Error creating child process.\n");
                exit(1);
            }
            if (pid == 0) {
                if (length(argVector, ARGVECTORSIZE) != 2) {
                    fprintf(stderr, "run must (only) receive <inputfile>.\n");
                    displayUsage(argv[0]);
                    exit(1);
                }
                char* execArgs[] =  {CH_APPNAME, argVector[1], NULL};
                int status = execv(CH_APPPATH, execArgs);
                if (status < 0) {
                    fprintf(stderr, "Error executing binary.\n");
                    exit(1);
                }
            } else {
                nChildren++;
            }
        } else if (command("exit", argVector)) {
            while (nChildren > 0) {
                waitAndSave(forks, &nChildren);
            }
             
            Process p;
            for (int i = 0; i < vector_getSize(forks); i++) {
                p = vector_at(forks, i);
                printf("CHILD EXITED (PID=%i; return %s)\n", p_getpid(p), 
                        p_getstatus(p) == 0 ? "OK" : "NOK");
            }
            break;
        } else {
            fprintf(stderr, "Invalid command %s\n", argVector[0]);
            displayUsage(argv[0]);
        }
    }

    return 0;
}
