/*
 * CircuitRouter-Client
 * João David, 89471
 * Gonçalo Almeida, 89448
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include "lib/types.h"
#include "lib/safecalls.h"
#include "shlib/shellprotocol.h"

#define DIRLENGTH 128


clientmsg_t buffer;
int fserv;
int fcli;


/*
 * displayUsage
 */
void displayUsage(const char* appName) {
    printf("Usage: %s PIPEPATH - Sends commands to AdvShell\n", appName);
    printf("    PIPEPATH <directory>  child processes limit   ULONG_MAX\n");
    printf("Shell:\n");
    printf("    Only the run <inputfile> command is allowed in the client.\n");
}


/*
 * parseArgs
 */
void parseArgs(int argc, char** argv) {
    if (argc != 2) {
        displayUsage(argv[0]);
        exit(EXIT_FAILURE);
    }
    if (access(argv[1], F_OK) == -1) {
        fprintf(stderr, "Invalid pipe path.\n");
        exit(EXIT_FAILURE);
    }
}


/*
 * endhandler
 */
void endhandler(int s) {
    int status = EXIT_SUCCESS;
    if (s == SIGPIPE) {
        char* msg = "Broken pipe, server probably not running.\n";
        write(2, msg, strlen(msg));
        status = EXIT_FAILURE;
    }

    if (close(fserv) < 0) {
        char* msg = "Error closing server pipe.";
        write(2, msg, strlen(msg));
        status = EXIT_FAILURE;
    }
 
    if (close(fcli) < 0) {
        // The fd might not be open, just ignore then
        if (errno != EBADF) {
            char* msg = "Error closing client pipe.";
            write(2, msg, strlen(msg));
            status = EXIT_FAILURE;
        }
    }

    if (unlink(buffer.pipe) < 0) {
        char* msg = "Error unlinking client pipe.";
        write(2, msg, strlen(msg));
        status = EXIT_FAILURE;
    }
    _exit(status);
}


/*
 * setsigaction: sets the default behaviour for a sigaction
 */
void setsigaction(int signum, struct sigaction* action, void* handler) {
    memset(action, 0, sizeof(struct sigaction));
    action->sa_handler = handler;
    safe_sigaction(signum, action, NULL);
}


/*
 * waitPipeClosed:Waits for the writer to close the pipe to make sure
 * when we reopen we get blocked until the Advanced Shell opens it again
 */
void waitPipeClosed(int fd) {
    char buff;
    while (read(fd, &buff, 1) > 0) {}
}

/*
 * createClientPipe: Creates the pipe for the Advanced Shell to use
 */
void createClientPipe() {
    char dir[DIRLENGTH];
    if ((getcwd(dir, DIRLENGTH)) == NULL) {
        perror("Error getting current working directory");
    }
    snprintf(buffer.pipe, MAXPIPELEN, "%s/.client%i.pipe", dir, getpid());
    safe_unlink(buffer.pipe);
    safe_mkfifo(buffer.pipe, 0666);
}


int main(int argc, char** argv) {
    puts("Welcome to the Advanced Shell Client.");
    parseArgs(argc, argv);


    // Set signal handlers
    struct sigaction pipeact;
    setsigaction(SIGPIPE, &pipeact, &endhandler);

    struct sigaction termact;
    setsigaction(SIGTERM, &termact, &endhandler);

    struct sigaction intact;
    setsigaction(SIGINT, &intact, &endhandler);

    // Open server pipe
    char* serverPipe = argv[1];
    fserv = safe_open(serverPipe, O_WRONLY);

    // Create client pipe
    createClientPipe();

    while (1) {
        printf("> ");
        char* line = fgets(buffer.msg, MAXMSGSIZE, stdin);
        if (line == NULL) {
            fprintf(stderr, "Error reading input or reached EOF, terminating.");
            exit(EXIT_FAILURE);
        }
   
        // Send any command to the advanced shell
        safe_write(fserv, &buffer, sizeof(buffer));
        
        // Wait for response and output it
        fcli = safe_open(buffer.pipe, O_RDONLY);
        int bread = safe_read(fcli, buffer.msg, MAXMSGSIZE);
        buffer.msg[bread] = '\0';
        waitPipeClosed(fcli);
        safe_close(fcli);
        printf("%s", buffer.msg);
    }
}
