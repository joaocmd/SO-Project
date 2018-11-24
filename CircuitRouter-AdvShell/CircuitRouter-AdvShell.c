/*
 * CircuitRouter-AdvShell.c
 * João David, 89471
 * Gonçalo Almeida, 89448
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include "commandparser.h"
#include "vector.h"
#include "types.h"
#include "chprocess.h"
#include "advshellprotocol.h"


unsigned int MAXCHILDREN = UINT_MAX;
#define CH_APPNAME "CircuitRouter-SeqSolver"
#define CH_APPPATH "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver"
#define FORKVECINITSIZE 4
#define ARGVECTORSIZE 4
#define BUFFERSIZE 256

bool_t forkGreenlight = FALSE;
int totalForks = 0;
int currForks = 0;
vector_t* forks;


/*
 * displayUsage
 */
void displayUsage(const char* appName) {
    printf("Usage: %s <optional argument>\n", appName);
    printf("Optional Argument:                                (default)\n");
    printf("    MAXCHILDREN <ULONG+>  child processes limit   ULONG_MAX\n");
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
        exit(EXIT_FAILURE);
    }
    if (argc == 2) {
        MAXCHILDREN = strtoul(argv[1], NULL, 10);    
        if (MAXCHILDREN == 0) {
            fprintf(stderr, "Invalid MAXCHILDREN.\n");
            displayUsage(argv[0]);
            exit(EXIT_FAILURE);
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
 * freeForks: frees the allocated memory for the finished processes vector.
 * Prints the pid and exit status of child processes if printProcesses is true.
 */
void freeForks(vector_t *forks, bool_t printProcesses) {
    process *proc;
    for (int i = 0; i < vector_getSize(forks); i++) {
        proc = vector_at(forks, i);
        if (printProcesses) {
            printf("CHILD EXITED (PID=%i; return %s)\n", p_getpid(proc), 
                    p_getstatus(proc) == OK ? "OK" : "NOK");
        }
        process_free(proc);
    }
    vector_free(forks);
}


/*
 * waitAndSave: waits for a process to finish. Creates a finished process 
 * and pushes it to the forks vector.
 */
void waitAndSave(vector_t *forks, int *nChildren) {
    pid_t pid;
    int status;
    pstatus_t pstatus = OK;
    pid = wait(&status);
    if (pid < 0) {
        if (errno == EINTR) {
            // Wait again because the wait was interrupted by signal.
            waitAndSave(forks, nChildren);
            return;
        } 
        fprintf(stderr, "Error waiting for child process.\n"); 
        freeForks(forks, FALSE);
        exit(EXIT_FAILURE);
    }
    if (WIFEXITED(status) == 0 || WEXITSTATUS(status) != 0) {
        pstatus = NOK;
    }
    process *proc = process_alloc(pid, "TODO");
    vector_pushBack(forks, proc);
    (*nChildren)--;
}


/*
 * createServerPipe: Creates and returns the file descriptor of a pipe
 * for the advanced shell to use.
 */
int createServerPipe(char* name) {
    int fd;
    unlink(name);
    if (mkfifo(name, 0666) < 0) {
        fprintf(stderr, "AdvShell: Couldn't create server pipe.\n");
        exit(EXIT_FAILURE);
    }
    if ((fd = open(name, O_RDONLY | O_NONBLOCK)) < 0) {
        fprintf(stderr, "AdvShell: Couldn't open server pipe.\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}

/*
 * invalidCommand: Treats an invalid command.
 */
void invalidCommand(int fd) {
    char* msg = "Command not supported.\n"; 
    write(fd, msg, strlen(msg) + 1);
}

/*
 * usr1handler: This signal (USR1) is used to tell the children when they can start.
 */
void usr1handler(int s) {
    //puts("let's go");
    forkGreenlight = TRUE;
}

/*
 * runCommand: Treats the run command.
 */
void runCommand(int fd, char* fdName, char** argVector, int nArgs) {
    if (nArgs != 2) {
        char* msg = "Run must (only) receive input file name.\n";
        write(fd, msg, strlen(msg) + 1);
        return;
    }

    // Should only run once (nCurrentChildren == MAXCHILDREN)
    while (currForks >= MAXCHILDREN) {
        pause();
        //waitAndSave(forks, &nChildren);//TODO
    }

    signal(SIGUSR1, &usr1handler);
    pid_t pid = fork(); 
    if (pid == -1) {
        fprintf(stderr, "Error creating child process.\n");
        freeForks(forks, FALSE);  
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        // Wait for parent to give green light signal.
        while (!forkGreenlight) {
            pause();
        }
        execl(CH_APPPATH, CH_APPNAME, argVector[1], NULL);
        // Only runs if execl failed
        freeForks(forks, FALSE); 
        exit(EXIT_FAILURE);
    } else {
        process* proc = process_alloc(pid, fdName); 
        vector_pushBack(forks, proc);
        p_start(proc);
        kill(pid, SIGUSR1);
        totalForks++;
        currForks++;
    }
}


/*
 * exitCommand: Treats the exit command.
 */
void exitCommand() {}


/*
 * treatClient: Treats the input coming from a client's buffer.
 */
void treatClient(char* buffer) {
    char* argVector[ARGVECTORSIZE];
    char clientPipe[MAXPIPELEN];
    int fcli;

    buffer = readTillChar(clientPipe, CLIMSGDELIM, buffer, BUFFERSIZE);
    int nArgs = parseCommand(argVector, ARGVECTORSIZE, buffer, BUFFERSIZE);
    if (nArgs == -1) {
        fprintf(stderr, "Error occured reading command, terminating.\n");
        exit(EXIT_FAILURE);
    }
    
    if ((fcli = open(clientPipe, O_WRONLY)) < 0) {
        fprintf(stderr, "AdvShell: Error opening client pipe\n");
        exit(EXIT_FAILURE);
    }

    /* Ignore empty prompts, we send a \0 to the client because
     * it is waiting for a response.
     */
    if (nArgs == 0) {
        write(fcli, "\0", 1);
        return;
    } else {
        if (command("run", argVector)) {
            runCommand(fcli, clientPipe, argVector, nArgs);            
        } else {
            invalidCommand(fcli);
        }
    }
    close(fcli);
}


/*
 * treatInput: Treats the input coming from advanced shell's input.
 */
void treatInput(char* buffer) {
    char* argVector[ARGVECTORSIZE];
    int nArgs = parseCommand(argVector, ARGVECTORSIZE, buffer, BUFFERSIZE);
    if (nArgs == -1) {
        fprintf(stderr, "Error occured reading command, terminating.\n");
        exit(EXIT_FAILURE);
    }

    // Ignore empty prompts
    if (nArgs == 0) return;
    if (command("run", argVector)) {
        runCommand(1, NULL, argVector, nArgs);            
    } else if (command("exit", argVector)) {
        fprintf(stdout, "Exiting shell...\n");
        while (currForks > 0) {
            pause();
            //waitAndSave(forks, &nChildren);
        }
        //freeForks(forks, TRUE);  
        //unlink(serverPipe);
        //break;
    } else {
        invalidCommand(2);
        //displayUsage(argv[0]);
    }
}


int main(int argc, char** argv) {
    parseArgs(argc, argv);

    // Child processes variables
    forks = vector_alloc(FORKVECINITSIZE);

    // Command reader buffer
    char buffer[BUFFERSIZE];

    // Create and open server pipe
    char serverPipe[MAXPIPELEN];
    int fserv;
    snprintf(serverPipe, MAXPIPELEN, "/tmp/%s.pipe", argv[0]);
    fserv = createServerPipe(serverPipe);

    fd_set smask;
    FD_ZERO(&smask);
    FD_SET(0, &smask);
    FD_SET(fserv, &smask);
    
    while (1) {
        fd_set tmask = smask; 
        int fready = select(fserv+1, &tmask, NULL, NULL, NULL);
        if (fready == -1) {
            fprintf(stderr, "AdvShell: Error waiting for communication.\n");
            exit(EXIT_FAILURE);
        }

        int bread;
        // Got input from stdin
        if (FD_ISSET(0, &tmask)) {
            bread = read(0, buffer, BUFFERSIZE);
            buffer[bread] = '\0';
            treatInput(buffer);
        }

        // Got a request from a client
        if (FD_ISSET(fserv, &tmask)) {
            bread = read(fserv, buffer, BUFFERSIZE);
            // If the connection is close read returns 0 bytes read
            if (bread == 0) {
                continue;
            }
            buffer[bread] = '\0';
            printf("%s", buffer);
            treatClient(buffer);
        }
    }
    exit(EXIT_SUCCESS);
}

