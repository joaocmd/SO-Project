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
    pstatus_t status;
    TIMER_T start;
    TIMER_T end;
};


/*
 * process_alloc: allocs memory and initializes a child process struct, 
 * returns it's pointer.
 */
process* process_alloc(pid_t pid);


/*
 * process_free: frees the allocated memory for a child process.
 */
void process_free(process* p);


/*
 * process_getpid: returns the PID of the child process.
 */
pid_t process_getpid(process* p);


/*
 * process_getstatus: returns the exit status of the child process.
 */
int process_getstatus(process* p);


/*
 * process_setstatus: sets the exit status of the child process.
 */
void process_setstatus(process* p, int status);


/*
 * process_start: registers the starting time for the process.
 */
void process_start(process* p);


/*
 * process_end: registers the ending time for the process.
 */
 void process_end(process* p);


/*
 * process_print: prints a process, appends newline.
 */
void process_print(process* p);

#endif
