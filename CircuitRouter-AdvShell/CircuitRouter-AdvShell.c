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
#include "lib/vector.h"
#include "lib/types.h"
#include "lib/commandparser.h"
#include "shlib/chprocess.h"
#include "shlib/shellprotocol.h"


unsigned int MAXCHILDREN = UINT_MAX;
#define CH_APPNAME "CircuitRouter-SeqSolver"
#define CH_APPPATH "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver"
#define FORKVECINITSIZE 4
#define ARGVECTORSIZE 4
#define BUFFERSIZE 256


bool_t forkGreenlight = FALSE;
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
 * printProcesses: Prints all processes in the forks vector.
 */
void printProcesses() {
    process *proc;
    for (int i = 0; i < vector_getSize(forks); i++) {
        proc = vector_at(forks, i);
        process_print(proc);
    }
    printf("END\n");
}


/*
 * findGoingProcess: returns the pointer to a child process
 * that has not yet completed (because they might have same PID if created
 * at different times) allocated in the forks vector.
 */
process* findGoingProcess(pid_t pid) {
    for (int i = 0; i < vector_getSize(forks); i++) {
        process* p = vector_at(forks, i);
        if (process_getpid(p) == pid && !process_isdone(p)) {
            return vector_at(forks, i);
        }
    }
    return NULL;
}


/*
 * cleanExit: Cleans allocated memory and exits
 */
void cleanExit(int status) {
    // Free processes
    process *proc;
    for (int i = 0; i < vector_getSize(forks); i++) {
        proc = vector_at(forks, i);
        process_free(proc);
    }
    vector_free(forks);
    exit(status);
}


/*
 * createServerPipe: Creates and returns the file descriptor of a pipe
 * for the advanced shell to use.
 */
int createServerPipe(char* name) {
    int fd;
    safe_unlink(name);
    safe_mkfifo(name, 0666);
    
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
    safe_write(fd, msg, strlen(msg) + 1);
}


/*
 * usr1handler: This signal (USR1) is used to tell the children when they can start.
 * By using this signal, we make sure that the child process only starts when its 
 * entry is registered in the forks vector, so that when a SIGCHLD is received we're
 * sure that it will be there.
 * Using this instead of SIGSTOP and SIGCONT because if parent gave SIGCONT before
 * child reaching SIGSTOP it would get stuck waiting for a signal.
 */
void sigusr1handler(int s) {
    forkGreenlight = TRUE;
}


/*
 * sigchldhandler: This signal is used to detect when a child process has probably
 * finished, because signals aren't queued, we have to check for multiple processes
 * in just one call.
 */
void sigchldhandler(int s) {
    int olderrno = errno;
    pid_t pid;
    int status;
    do {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                char* waitErrorMsg = "Error waiting for child process.\n";
                safe_write(2, waitErrorMsg, strlen(waitErrorMsg) + 1); 
                cleanExit(EXIT_FAILURE);
            }
        } 
        if (pid > 0) {
            // We're sure the process is there, so no need to check return value
            process* proc = findGoingProcess(pid);
            process_end(proc);
            process_setstatus(proc, status); 
            currForks--;
        }
    } while (pid != 0);
    errno = olderrno;
}


/*
 * runCommand: Treats the run command.
 */
void runCommand(int fd, char** argVector, int nArgs) {
    if (nArgs != 2) {
        char* msg = "Run must (only) receive input file name.\n";
        safe_write(fd, msg, strlen(msg) + 1);
        return;
    }

    while (currForks >= MAXCHILDREN) {
        pause();
    }

    pid_t pid = fork(); 
    if (pid == -1) {
        fprintf(stderr, "Error creating child process.\n");
        cleanExit(EXIT_FAILURE);
    }
    if (pid == 0) {
        // Wait for parent to give green light signal.
        while (!forkGreenlight) {
            pause();
        }

        if (fd != 1) {
            safe_dup2(fd, 1);
            safe_dup2(fd, 2);
            safe_close(fd);
        }
        execl(CH_APPPATH, CH_APPNAME, argVector[1], NULL);
        // Only runs if execl failed
        fprintf(stderr, "Error executing %s.\n", CH_APPNAME);
        cleanExit(EXIT_FAILURE);
    } else {
        process* proc = process_alloc(pid); 
        if (proc == NULL) {
            fprintf(stderr, "Error allocating object");
            exit(1);
        }
        vector_pushBack(forks, proc);
        process_start(proc);
        kill(pid, SIGUSR1);
        currForks++;
    }
}


/*
 * exitCommand: Treats the exit command.
 */
void exitCommand() {
    while (currForks > 0) {
        pause();
    }
    printProcesses();
    //TODO
    //unlink(serverPipe);
    //close;
    cleanExit(EXIT_SUCCESS);
}


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
    
    safe_open(clientPipe, O_WRONLY);


    /* Ignore empty prompts, we send a \0 to the client because
     * it is waiting for a response.
     */
    if (nArgs == 0) {
        safe_write(fcli, "\0", 1);
        return;
    } else {
        if (command("run", argVector)) {
            puts("Client run request received.");
            runCommand(fcli, argVector, nArgs);            
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
        runCommand(1, argVector, nArgs);            
    } else if (command("exit", argVector)) {
        exitCommand();
    } else {
        invalidCommand(2);
        //displayUsage(argv[0]);
    }
}


int main(int argc, char** argv) {
    parseArgs(argc, argv);
    
    signal(SIGCHLD, &sigchldhandler);
    signal(SIGUSR1, &sigusr1handler);

    /*
    struct sigaction usr1act;
    memset(&usr1act, 0, sizeof(usr1act));
    usr1act.sa_handler = &sigusr1handler;
    sigaction(SIGUSR1, &usr1act, NULL);
    
    struct sigaction chldact;
    memset(&chldact, 0, sizeof(chldact));
    usr1act.sa_handler = &sigchldhandler;
    sigaction(SIGCHLD, &chldact, NULL);*/

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
            if (errno == EINTR) {
                continue;
            }
            fprintf(stderr, "AdvShell: Error waiting for communication.\n");
            exit(EXIT_FAILURE);
        }

        int bread;
        // Got input from stdin
        if (FD_ISSET(0, &tmask)) {
            bread = safe_read(0, buffer, BUFFERSIZE);
            buffer[bread] = '\0';
            treatInput(buffer);
        }

        // Got a request from a client
        if (FD_ISSET(fserv, &tmask)) {
            bread = safe_read(fserv, buffer, BUFFERSIZE);
            // If the connection is close read returns 0 bytes read
            if (bread == 0) {
                continue;
            }
            buffer[bread] = '\0';
            treatClient(buffer);
        }
    }
    exit(EXIT_SUCCESS);
}

