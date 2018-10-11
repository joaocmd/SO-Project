/*
* finprocess.h
* João David, 89471
* Gonçalo Almeida, 89448
*/

#ifndef FIN_PROCESS_H
#define FIN_PROCESS_H 1

#include <sys/types.h>

typedef enum {OK, NOK} status_t;

/*
 * Represents a process that has finished.
*/
typedef struct fprocess process;
struct fprocess {
    pid_t pid;
    status_t status;
};

/*
 * process_alloc: allocs memory and initializes a finished process struct, 
 * returns it's pointer.
*/
process *process_alloc(pid_t pid, status_t status);

/*
 * process_free: frees the allocated memory for a finished process.
*/
void process_free(process *p);

/*
 * p_getpid: returns the PID of the finished process.
*/
pid_t p_getpid(process *p);

/*
 * p_getstatus: returns the exit status of the finished process.
*/
int p_getstatus(process *p);

#endif
