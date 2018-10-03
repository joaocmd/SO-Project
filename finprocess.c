/*
* finprocess.c
* João David, 89471
* Gonçalo Almeida, 89448
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "finprocess.h"

Process process_alloc(pid_t pid, int status) {
    Process p = malloc(sizeof(struct fprocess));
    p->pid = pid;
    p->status = status;
    return p;
}

void free_process(Process p) {
    free(p);
}

pid_t p_getpid(Process p) {
    return p->pid;
}

int p_getstatus(Process p) {
    return p->status;
}
