/*
* finprocess.c
* João David, 89471
* Gonçalo Almeida, 89448
*/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "chprocess.h"
#include "advshellprotocol.h"
#include "timer.h"

#define PRINTBUFFSIZE

/*
 * process_alloc: allocs memory and initializes a child process struct, 
 * returns it's pointer.
 */
process* process_alloc(pid_t pid) {
    process *p = malloc(sizeof(process*));
    p->pid = pid;
    return p;
}


/*
 * process_free: frees the allocated memory for a child process.
 */
void process_free(process* p) {
    free(p);
}


/*
 * process_getpid: returns the PID of the child process.
 */
pid_t process_getpid(process* p) {
    return p->pid;
}


/*
 * process_getstatus: returns the exit status of the child process.
 */
int process_getstatus(process* p) {
    return p->status;
}


/*
 * process_setstatus: sets the exit status of the child process.
 */
void process_setstatus(process* p, int status) {
    pstatus_t s = OK;
    if (WIFEXITED(status) == 0 || WEXITSTATUS(status != 0)) {
        s = NOK;
    }
    p->status = s;
}
/*
 * process_start: registers the starting time for the process.
 */
void process_start(process* p) {
    TIMER_READ(p->start);
}

/*
 * process_end: registers the ending time for the process.
 */
void process_end(process* p) {
    TIMER_READ(p->end);
}


/*
 * process_print: prints a process, appends newline.
 */
void process_print(process* p) {
    char* status = p->status == OK? "OK" : "NOK";
    int time = TIMER_DIFF_SECONDS(p->start, p->end);
    printf("CHILD EXITED (PID=%i; return %s; %i s)\n", p->pid, status, time);
}
