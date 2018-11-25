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
#include "lib/safecalls.h"
#include "shlib/shellprotocol.h"

#define BUFFERSIZE 256
#define MSGLENGTH (MAXPIPELEN+BUFFERSIZE+1)



/*
 * displayWelcome
 */
void displayWelcome() {
    puts("Welcome to the Advanced Shell Client.");
    puts("This client can send commands to the Advanced Shell.");
    puts("After sending a request, wait for a response, the shell");
    puts("will show a '>' when it's ready for input");
}


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
 * command: returns whether the command matches the first argument in argVector.
 */
int command(const char* command, char** argVector) {
    return strcmp(argVector[0], command) == 0;
}


int main(int argc, char** argv) {
    displayWelcome();
    parseArgs(argc, argv);

    // Reader buffer
    char buffer[BUFFERSIZE];

    // Open server pipe
    char* serverPipe = argv[1];
    int fserv;
    fserv = safe_open(serverPipe, O_WRONLY);

    // Create and open client pipe
    char clientPipe[MAXPIPELEN]; 
    int fcli;
    snprintf(clientPipe, MAXPIPELEN, "/tmp/advclient%i.pipe", getpid());
    safe_unlink(clientPipe);
    safe_mkfifo(clientPipe, 0666);

    while (1) {
        printf("> ");
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
        
        // Wait for response and output it
        fcli = safe_open(clientPipe, O_RDONLY);
        int bread = read(fcli, buffer, BUFFERSIZE);
        safe_close(fcli);
        buffer[bread] = '\0';
        printf("%s", buffer);
    }

    exit(EXIT_SUCCESS);
}

