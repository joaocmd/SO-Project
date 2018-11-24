/*
* finprocess.c
* João David, 89471
* Gonçalo Almeida, 89448
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "chprocess.h"
#include "advshellprotocol.h"
#include "timer.h"


/*
 * process_alloc: allocs memory and initializes a child process struct, 
 * returns it's pointer.
 */
process* process_alloc(pid_t pid, char* pipe) {
    process *p = malloc(sizeof(process*));
    p->pid = pid;
    if(pipe != NULL) {
        strncpy(p->pipe, pipe, MAXPIPELEN);
    }
    return p;
}


/*
 * process_free: frees the allocated memory for a child process.
 */
void process_free(process* p) {
    free(p);
}


/*
 * p_getpid: returns the PID of the child process.
 */
pid_t p_getpid(process* p) {
    return p->pid;
}


/*
 * p_getstatus: returns the exit status of the child process.
 */
int p_getstatus(process* p) {
    return p->status;
}


/*
 * p_start: registers the starting time for the process.
 */
void p_start(process* p) {
    TIMER_READ(p->start);
}

/*
 * p_end: registers the ending time for the process.
 */
 void p_end(process* p) {
    TIMER_READ(p->end);
 }
