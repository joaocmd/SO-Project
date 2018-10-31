#ifndef LOCKSGRID_H
#define LOCKSGRID_H

#include <pthread.h>

//TODO comments and cleaning up, maybe allign cache.

typedef struct locksgrid {
    long long dimension;
    long width;
    long height;
    long depth;
    pthread_mutex_t* locks;
} locksgrid_t;

locksgrid_t* locksgrid_alloc(long width, long height, long depth);

void locksgrid_free(locksgrid_t* lgrid);

pthread_mutex_t* locksgrid_getLock(locksgrid_t* lgrid, long i);

#endif
