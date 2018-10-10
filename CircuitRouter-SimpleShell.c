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
#include "lib/types.h"
#include "finprocess.h"

unsigned int MAXCHILDREN = UINT_MAX;
#define CH_APPNAME "CircuitRouter"
#define CH_APPPATH "CircuitRouter-SeqSolver/CircuitRouter-SeqSolver"
#define ARGVECTORSIZE 4
#define BUFFERSIZE 256

/*
 * displayUsage
*/
void displayUsage(const char* appName) {
    printf("Usage: %s\n", appName);
    printf("Additional Argument:                              (default)\n");
    printf("    MAXCHILDREN <ULONG>   child processes limit   ULONG_MAX\n");
    printf("Shell:\n");
    printf("    run <inputfile>       runs %s with <inputfile>\n", CH_APPNAME);
    printf("    exit                  exits the console\n");
}

/*
 * parseArgs
*/
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

/*
 * command: returns whether the command matches the first argument in argVector.
*/
int command(const char* command, char** argVector) {
    return strcmp(argVector[0], command) == 0;
}

/*
 * length: returns how many non-NULL entries are in vector v.
*/
int length(char **v, int vCapacity) { //TODO ficheiro diferente
    int i;
    for (i = 0; i < vCapacity; i++) {
        if (v[i] == NULL) {
            break;
        }
    }
    return i;
}

/*
 * waitAndSave: waits for a process to finish. Creates a finished process 
 * and pushes it to the forks vector.
*/
void waitAndSave(vector_t *forks, int *nChildren) {
    pid_t pid;
    int status;
    pid = wait(&status);
    process *proc = process_alloc(pid, status);
    vector_pushBack(forks, proc);
    (*nChildren)--;
}

/*
 * freeForks: frees the allocated memory for the finished processes vector.
 * Prints the pid and exit status of child processes if printProcesses is true.
*/
void freeForks(vector_t *forks, bool_t printProcesses) {
    process *proc;
    for (int i = 0; i < vector_getSize(forks); i++) {
        proc = vector_at(forks, i);
        if (printProcesses) {
            printf("CHILD EXITED (PID=%i; return %s)\n", p_getpid(proc), 
                    p_getstatus(proc) == 0 ? "OK" : "NOK");
        }
        process_free(proc);
    }
    vector_free(forks);
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
                freeForks(forks, FALSE);  
                exit(1);
            }
            if (pid == 0) {
                if (length(argVector, ARGVECTORSIZE) != 2) {
                    fprintf(stderr, "run must (only) receive <inputfile>.\n");
                    displayUsage(argv[0]);
                    freeForks(forks, FALSE);  //TODO necessario?
                    exit(1);
                }
                char* execArgs[] =  {CH_APPNAME, argVector[1], NULL};
                execv(CH_APPPATH, execArgs);
                // Only runs if execv failed
                freeForks(forks, FALSE);  
                exit(1);
                }
            } else {
                nChildren++;
            }
        } else if (command("exit", argVector)) {
            while (nChildren > 0) {
                waitAndSave(forks, &nChildren);
            }
            freeForks(forks, TRUE);  
            break;
        } else {
            fprintf(stderr, "Invalid command %s\n", argVector[0]);
            displayUsage(argv[0]);
        }
    }

    exit(0);
}
