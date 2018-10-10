/*
* finprocess.c
* João David, 89471
* Gonçalo Almeida, 89448
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "finprocess.h"

process *process_alloc(pid_t pid, int status) {
    process *p = malloc(sizeof(struct fprocess));
    p->pid = pid;
    p->status = status;
    return p;
}

void process_free(process *p) {
    free(p);
}

pid_t p_getpid(process *p) {
    return p->pid;
}

int p_getstatus(process *p) {
    return p->status;
}
