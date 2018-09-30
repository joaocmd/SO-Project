/*
* CircuitRouter-SimpleShell
* João David, 89471
* Gonçalo Almeida, 89448
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "../lib/commandlinereader.h"

int main(int argc, char** argv) {

    unsigned long MAXCHILDREN = ULONG_MAX;
    if (argc > 1) {
         
    }

    if (pid == -1) {
        fprintf(stderr, "Error creating child process\n");
        exit(1);
    }
    
    if (pid == 0) {
        printf("I'm child process with pid = %i\n", getpid());
    } else {
        printf("I'm parent process with pid = %i, my child is %i\n", getpid(), pid);
    }


    return 0;
}
