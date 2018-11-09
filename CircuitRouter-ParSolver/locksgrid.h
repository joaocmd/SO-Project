#ifndef LOCKSGRID_H
#define LOCKSGRID_H

#include <pthread.h>


typedef struct locksgrid {
    long long dimension;
    long width;
    long height;
    long depth;
    pthread_mutex_t* locks;
} locksgrid_t;

/* =============================================================================
 * locksgrid_alloc
 * =============================================================================
 */
locksgrid_t* locksgrid_create(long width, long height, long depth);


/* =============================================================================
 * locksgrid_free
 * =============================================================================
 */
void locksgrid_free(locksgrid_t* lgrid);


/* =============================================================================
 * locksgrid_getLock
 * =============================================================================
 */
pthread_mutex_t* locksgrid_getLock(locksgrid_t* lgrid, long i);

#endif
