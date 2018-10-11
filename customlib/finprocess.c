/*
* finprocess.c
* João David, 89471
* Gonçalo Almeida, 89448
*/

#include "finprocess.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*
 * process_alloc: allocs memory and initializes a finished process struct, 
 * returns it's pointer.
*/
process *process_alloc(pid_t pid, status_t status) {
    process *p = malloc(sizeof(struct fprocess));
    p->pid = pid;
    p->status = status;
    return p;
}

/*
 * process_free: frees the allocated memory for a finished process.
*/
void process_free(process *p) {
    free(p);
}

/*
 * p_getpid: returns the PID of the finished process.
*/
pid_t p_getpid(process *p) {
    return p->pid;
}

/*
 * p_getstatus: returns the exit status of the finished process.
*/
int p_getstatus(process *p) {
    return p->status;
}
