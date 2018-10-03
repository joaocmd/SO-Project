/*
* finprocess.h
* João David, 89471
* Gonçalo Almeida, 89448
*/

#ifndef FIN_PROCESS_H
#define FIN_PROCESS_H 1

#include <sys/types.h>

typedef struct fprocess *Process;
// Represents a process that has finished.
struct fprocess {
    pid_t pid;
    int status;
};

// process_alloc: allocs memory and initializes a finished process struct, 
// returns it's pointer.
Process process_alloc(pid_t pid, int status);

// free_process: frees the allocated memory for a finished process.
void free_process(Process p);

// p_getpid: returns the PID of the finished process.
pid_t p_getpid(Process p);

// p_getstatus: returns the exit status of the finished process.
int p_getstatus(Process p);

#endif
