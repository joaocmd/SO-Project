/*
* finprocess.h
* João David, 89471
* Gonçalo Almeida, 89448
*/

#ifndef CHPROCESS_H
#define CHPROCESS_H 1

#include <sys/types.h>
#include "advshellprotocol.h"
#include "timer.h"

typedef enum {OK, NOK} pstatus_t;

/*
 * Represents a process a process used by advanced shell.
 */
typedef struct chprocess process;
struct chprocess {
    pid_t pid;
    char pipe[MAXPIPELEN];
    pstatus_t status;
    TIMER_T start;
    TIMER_T end;
};

/*
 * process_alloc: allocs memory and initializes a child process struct, 
 * returns it's pointer.
 */
process* process_alloc(pid_t pid, char* pipe);

/*
 * process_free: frees the allocated memory for a child process.
 */
void process_free(process* p);

/*
 * p_getpid: returns the PID of the child process.
 */
pid_t p_getpid(process* p);

/*
 * p_getstatus: returns the exit status of the child process.
 */
int p_getstatus(process* p);

/*
 * p_start: registers the starting time for the process.
 */
void p_start(process* p);

/*
 * p_end: registers the ending time for the process.
 */
 void p_end(process* p);
#endif
