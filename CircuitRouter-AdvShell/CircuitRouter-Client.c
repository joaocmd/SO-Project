/*
 * CircuitRouter-Client
 * João David, 89471
 * Gonçalo Almeida, 89448
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include "../lib/types.h"
#include "advshellprotocol.h"

#define BUFFERSIZE 256

/*
 * displayUsage
 */
void displayUsage(const char* appName) {
    printf("Usage: %s PIPEPATH - Sends commands to AdvShell\n", appName);
    printf("    PIPEPATH <directory>  child processes limit   ULONG_MAX\n");
    printf("Shell:\n");
    printf("    exitclient            exits the client\n");
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
 * command: returns whether the command matches the first argument in argVector.
 */
int command(const char* command, char** argVector) {
    return strcmp(argVector[0], command) == 0;
}


int main(int argc, char** argv) {
    parseArgs(argc, argv);

    // Reader buffer
    char buffer[BUFFERSIZE];

    char* serverPipe = argv[1];
    int fserv;
    // Open server pipe
    if ((fserv = open(serverPipe, O_WRONLY)) < 0) {
        fprintf(stderr, "Client: couldn't open AdvShell pipe.\n");
        exit(EXIT_FAILURE);
    }

    char clientPipe[MAXPIPELEN]; 
    int fcli;
    // Create and open client pipe
    snprintf(clientPipe, MAXPIPELEN, "/tmp/advclient%i.pipe", getpid());
    unlink(clientPipe);
    if (mkfifo(clientPipe, 0666) < 0) {
        fprintf(stderr, "Client: couldn't create client pipe.\n");
        exit(EXIT_FAILURE);
    }
    if ((fcli = open(clientPipe, O_RDONLY | O_NONBLOCK)) < 0) {
        fprintf(stderr, "Client: couldn't open client pipe.\n");
        exit(EXIT_FAILURE);
    }

    fd_set cmask;
    FD_ZERO(&cmask);
    FD_SET(0, &cmask);
    FD_SET(fcli, &cmask);

    while (1) {
        fd_set tmask = cmask;

        int fready = select(fcli+1, &tmask, NULL, NULL, NULL);
        if (fready == -1) {
            fprintf(stderr, "Client: Error waiting for communication.\n");
            exit(1);
        }
        printf("%i fds ready\n", fready);

        // Got input from stdin
        if (FD_ISSET(0, &tmask)) {
            puts("Stdin fd ready");
            char* line = fgets(buffer, BUFFERSIZE, stdin);
            if (line == NULL) {
                fprintf(stderr, "%s\n", buffer);
                fprintf(stderr, "Error reading input, terminating.\n");
                exit(EXIT_FAILURE);
            }
       
            // Send any command to the advanced shell.
            char msg[BUFFERSIZE];
            snprintf(msg, BUFFERSIZE, "%s%c%s", clientPipe, CLIMSGDELIM, buffer);
            write(fserv, msg, strlen(msg) + 1);
        }
        
        // Got a response from a process
        if (FD_ISSET(fcli, &tmask)) {
            char msg[BUFFERSIZE];
            int bread = read(fcli, msg, BUFFERSIZE);
            msg[bread] = '\0';
            printf("%i\n%s", bread, msg);
        }

        sleep(1);
    }

    exit(EXIT_SUCCESS);
}
