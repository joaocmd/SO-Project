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

    // Create and open client pipe
    snprintf(buffer.pipe, MAXPIPELEN, ".advclient%i.pipe", getpid());
    safe_unlink(buffer.pipe);
    safe_mkfifo(buffer.pipe, 0666);

    while (1) {
        printf("> ");
        char* line = fgets(buffer.msg, MAXMSGSIZE, stdin);
        if (line == NULL) {
            fprintf(stderr, "Error reading input, terminating.\n");
            exit(EXIT_FAILURE);
        }
   
        // Send any command to the advanced shell
        write(fserv, &buffer, sizeof(buffer));
        
        // Wait for response and output it
        // TODO wait for adv shell to close before trying to open
        fcli = safe_open(buffer.pipe, O_RDONLY);
        int bread = read(fcli, buffer.msg, MAXMSGSIZE);
        safe_close(fcli);
        buffer.msg[bread] = '\0';
        printf("%s", buffer.msg);
    }
}
