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
#include "lib/types.h"
#include "shlib/shellprotocol.h"

#define BUFFERSIZE 256
#define MSGLENGTH (MAXPIPELEN+BUFFERSIZE+1)

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

    while (1) {
        char* line = fgets(buffer, BUFFERSIZE, stdin);
        if (line == NULL) {
            fprintf(stderr, "%s\n", buffer);
            fprintf(stderr, "Error reading input, terminating.\n");
            exit(EXIT_FAILURE);
        }
   
        // Send any command to the advanced shell
        char msg[MSGLENGTH];
        snprintf(msg, MSGLENGTH, "%s%c%s", clientPipe, CLIMSGDELIM, buffer);
        write(fserv, msg, strlen(msg) + 1);
        
        if ((fcli = open(clientPipe, O_RDONLY)) < 0) {
            fprintf(stderr, "Client: couldn't open client pipe.\n");
            exit(EXIT_FAILURE);
        }

        // Wait for response and output it
        int bread = read(fcli, buffer, BUFFERSIZE);
        close(fcli);
        buffer[bread] = '\0';
        printf("%s", buffer);
    }

    exit(EXIT_SUCCESS);
}

