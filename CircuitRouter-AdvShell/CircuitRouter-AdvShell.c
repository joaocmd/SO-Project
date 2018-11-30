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
#include "lib/safecalls.h"
#include "shlib/chprocess.h"
#include "shlib/shellprotocol.h"


unsigned int MAXCHILDREN = UINT_MAX;
#define CH_APPNAME "CircuitRouter-SeqSolver"
#define CH_APPPATH "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver"
#define FORKVECINITSIZE 4
#define ARGVECTORSIZE 4
#define BUFFERSIZE MAXMSGSIZE


sigset_t sigchldmask;
int currForks = 0;
vector_t* forks;


int fserv;
char serverPipe[MAXPIPELEN];



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
 * cleanExit: Cleans allocated memory and exits, doesn't remove the pipe
 * because this function is also used by child processes.
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
 * createServerPipe: Creates the server pipe for the clients to use
 */
void createServerPipe(char* name) {
    safe_unlink(name);
    safe_mkfifo(name, 0666);
}


/*
 * invalidCommand: Treats an invalid command.
 */
void invalidCommand(int fd) {
    char* msg = "Command not supported.\n"; 
    safe_write(fd, msg, strlen(msg) + 1);
}


/*
 * sigchldhandler: This signal is used to detect when a child process has probably
 * finished, because standard signals (such as SIGCHLD) aren't queued, we have to 
 * check for multiple processes in just one call.
 */
void sigchldhandler(int s) {
    int olderrno = errno;
    pid_t pid;
    int status;
    do {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid < 0) {
            if (errno != ECHILD) {
                char* waitErrorMsg = "Error waiting for child process.\n";
                write(2, waitErrorMsg, strlen(waitErrorMsg) + 1); 
                // Can't call cleanExit because it is not async-signal-safe
                if (unlink(serverPipe) < 0) {
                    char* unlinkErrorMsg = "Error deleting server pipe.\n";
                    write(2, unlinkErrorMsg, strlen(unlinkErrorMsg) + 1);
                }
                _exit(EXIT_FAILURE);
            }
        } 
        if (pid > 0) {
            // We're sure the process is there, so no need to check return value
            process* proc = findGoingProcess(pid);
            process_end(proc);
            process_setstatus(proc, status); 
            currForks--;
        }
    } while (pid > 0);
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

    if (currForks >= MAXCHILDREN) {
        puts("More forks running than allowed, waiting...");
    }
    while (currForks >= MAXCHILDREN) {}

    pid_t pid = fork(); 
    if (pid == -1) {
        perror("Error creating child process");
        cleanExit(EXIT_FAILURE);
    }
    if (pid == 0) {
        if (fd != 1) {
            safe_dup2(fd, 1);
            safe_dup2(fd, 2);
            safe_close(fd);
        }
        execl(CH_APPPATH, CH_APPNAME, argVector[1], NULL);
        // Only runs if execl failed
        perror("Error on exec call");
        cleanExit(EXIT_FAILURE);
    } else {
        // Blocking signals because we have to make sure
        // the new process is in the forks vector before 
        // treating any SIGCHLD, also protects currForks
        safe_sigprocmask(SIG_BLOCK, &sigchldmask, NULL);
        process* proc = process_alloc(pid); 
        if (proc == NULL) {
            fprintf(stderr, "Error allocating object");
            cleanExit(EXIT_FAILURE);
        }
        vector_pushBack(forks, proc);
        currForks++;
        process_start(proc);
        safe_sigprocmask(SIG_UNBLOCK, &sigchldmask, NULL);
    }
}


/*
 * exitCommand: Treats the exit command.
 */
void exitCommand() {
    puts("Exiting Advanced Shell");
    while (currForks > 0) {}
    printProcesses();
    safe_close(fserv);
    safe_unlink(serverPipe);
    cleanExit(EXIT_SUCCESS);
}


/*
 * treatClient: Treats the input coming from a client's buffer.
 */
void treatClient(clientmsg_t* buffer) {
    char* argVector[ARGVECTORSIZE];
    int fcli;

    int nArgs = parseCommand(argVector, ARGVECTORSIZE, buffer->msg, MAXMSGSIZE);
    if (nArgs == -1) {
        perror("Error occured reading command, terminating");
        exit(EXIT_FAILURE);
    }
    

    fcli = safe_open(buffer->pipe, O_WRONLY);
    /* Ignore empty prompts, we send a \0 to the client because
     * it is waiting for a response.
     */
    if (nArgs == 0) {
        safe_write(fcli, "\0", 1);
    } else {
        if (command("run", argVector)) {
            puts("Client run request received.");
            runCommand(fcli, argVector, nArgs);            
        } else {
            invalidCommand(fcli);
        }
    }
    safe_close(fcli);
}


/*
 * treatInput: Treats the input coming from advanced shell's input.
 */
void treatInput(char* buffer) {
    char* argVector[ARGVECTORSIZE];
    int nArgs = parseCommand(argVector, ARGVECTORSIZE, buffer, BUFFERSIZE);

    if (nArgs == -1) {
        perror("Error occured reading command, terminating");
        exit(EXIT_FAILURE);
    }

    // Ignore empty prompts
    if (nArgs == 0) {
        return;
    }
    if (command("run", argVector)) {
        runCommand(1, argVector, nArgs);            
    } else if (command("exit", argVector)) {
        exitCommand();
    } else {
        invalidCommand(2);
    }
}


int main(int argc, char** argv) {
    puts("Welcome to Advanced Shell");
    parseArgs(argc, argv);

    // Prepare mask to block SIGCHLD
    // No need to check errors because EINVAL won't ever happen
    sigemptyset(&sigchldmask);
    sigaddset(&sigchldmask, SIGCHLD);
    
    // Set SIGCHLD action
    struct sigaction chldact;
    memset(&chldact, 0, sizeof(struct sigaction));
    chldact.sa_handler = &sigchldhandler;
    chldact.sa_flags = SA_NOCLDSTOP;
    safe_sigaction(SIGCHLD, &chldact, NULL);

    // Child processes variables
    forks = vector_alloc(FORKVECINITSIZE);

    // Command reader buffers
    char buffer[BUFFERSIZE];
    clientmsg_t cliBuffer;

    // Create and open server pipe
    snprintf(serverPipe, MAXPIPELEN, "%s.pipe", argv[0]);
    createServerPipe(serverPipe);
    fserv = safe_open(serverPipe, O_RDONLY | O_NONBLOCK);

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
            perror("AdvShell: Error waiting for communication");
            exit(EXIT_FAILURE);
        }

        // Got input from stdin
        if (FD_ISSET(0, &tmask)) {
            if (fgets(buffer, BUFFERSIZE, stdin) == NULL) {
                fprintf(stderr, "Error reading input or reached EOF, terminating.\n");
            }
            treatInput(buffer);
        }

        // Got a request from a client
        if (FD_ISSET(fserv, &tmask)) {
            // Connection closed, reopening pipe to avoid actively trying
            // to read because of false positives given by select
            if (safe_read(fserv, &cliBuffer, sizeof(cliBuffer)) == 0) {
                safe_close(fserv);
                fserv = safe_open(serverPipe, O_RDONLY | O_NONBLOCK);
                continue;
            }
            treatClient(&cliBuffer);
        }
    }
}
