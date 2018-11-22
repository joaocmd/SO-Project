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
#include "../lib/commandlinereader.h"
#include "../lib/types.h"

#define ARGVECTORSIZE 4
#define BUFFERSIZE 256

#define MAXPIPELEN 64

/*
 * displayUsage
 */
static void displayUsage(const char* appName) {
    printf("Usage: %s PIPEPATH - Sends commands to AdvShell\n", appName);
    printf("    PIPEPATH <directory>  child processes limit   ULONG_MAX\n");
    printf("Shell:\n");
    printf("    exitclient            exits the client\n");
}

/*
 * parseArgs
 */
static void parseArgs(int argc, char** argv) {
    if (argc != 2) {
        displayUsage(argv[0]);
        exit(1);
    }
    if (access(argv[1], F_OK) == -1) {
        fprintf(stderr, "Invalid pipe path.\n");
    }
}

/*
 * command: returns whether the command matches the first argument in argVector.
 */
static int command(const char* command, char** argVector) {
    return strcmp(argVector[0], command) == 0;
}

int main(int argc, char** argv) {
    parseArgs(argc, argv);
    char *argVector[ARGVECTORSIZE];
    char buffer[BUFFERSIZE];

    char* serverPipe = argv[1];
    int fserv;
    // Open server pipe
    if ((fserv = open(serverPipe, O_WRONLY)) < 0) {
        fprintf(stderr, "Client: couldn't open AdvShell pipe.\n");
        exit(1);
    }

    char clientPipe[MAXPIPELEN]; 
    int fcli;
    // Open client pipe
    sprintf(clientPipe, ".ClientPipe%li.pipe", getpid());
    unlink(clientPipe);
    if (mkfifo(clientPipe, 0666) < 0) {
        fprintf(stderr, "Client: couldn't create client pipe.\n");
        exit(1);
    }
    if ((fcli = open(clientPipe, O_RDONLY)) < 0) {
        fprintf(stderr, "Client: couldn't open client pipe.\n");
        exit(1);
    }

    while (1) {
        int nArgs = readLineArguments(argVector, ARGVECTORSIZE, buffer, BUFFERSIZE);
        if (nArgs == -1) {
            fprintf(stderr, "Error occured reading command, terminating.\n");
            exit(1);
	    }
        puts("AAA");

        // Ignore empty prompts
        if (nArgs == 0) { continue; }
        if (command("exitclient", argVector)) {
            break;
        } else {
            // Send any command to the advanced shell.
            char* msg;
            sscanf(msg, "%s|%s\n", clientPipe, buffer);
            write(fserv, msg, sizeof(char)*strlen(msg));
        }
    }

    exit(0);
}
