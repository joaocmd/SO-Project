#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "locksgrid.h"

locksgrid_t* locksgrid_alloc(long width, long height, long depth) {

    locksgrid_t* lgrid = malloc(sizeof(locksgrid_t));

    if (lgrid) {
        lgrid->width = width;
        lgrid->height = height;
        lgrid->depth = depth;
        lgrid->dimension = width * height * depth;
        pthread_mutex_t* locks = malloc(sizeof(pthread_mutex_t) * lgrid->dimension);
        assert(locks);
        for (long long i = 0; i < lgrid->dimension; i++) {
            assert(pthread_mutex_init(&locks[i], NULL) == 0);
        }

        lgrid->locks = locks;
    }
    return lgrid;
}

void locksgrid_free(locksgrid_t* lgrid) {
    for (long long i = 0; i < lgrid->dimension; i++) {
        pthread_mutex_destroy(&lgrid->locks[i]);
    }
    free(lgrid->locks);
    free(lgrid);
}

pthread_mutex_t* locksgrid_getLock(locksgrid_t* lgrid, long i) {
    return &(lgrid->locks[i]);
}
