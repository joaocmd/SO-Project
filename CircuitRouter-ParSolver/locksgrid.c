#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "locksgrid.h"
#include "mutexutils.h"

/* =============================================================================
 * locksgrid_alloc
 * =============================================================================
 */
locksgrid_t* locksgrid_create(long width, long height, long depth) {

    locksgrid_t* lgrid = malloc(sizeof(locksgrid_t));

    if (lgrid) {
        lgrid->width = width;
        lgrid->height = height;
        lgrid->depth = depth;
        lgrid->dimension = (long long) width * height * depth;
        pthread_mutex_t* locks = malloc(sizeof(pthread_mutex_t) * lgrid->dimension);
        assert(locks);
        for (long long i = 0; i < lgrid->dimension; i++) {
            mutils_init(&locks[i]);
        }

        lgrid->locks = locks;
    }
    return lgrid;
}


/* =============================================================================
 * locksgrid_free
 * =============================================================================
 */
void locksgrid_free(locksgrid_t* lgrid) {
    for (long long i = 0; i < lgrid->dimension; i++) {
        mutils_destroy(&lgrid->locks[i]);
    }
    free(lgrid->locks);
    free(lgrid);
}


/* =============================================================================
 * locksgrid_getLock
 * =============================================================================
 */
pthread_mutex_t* locksgrid_getLock(locksgrid_t* lgrid, long i) {
    return &(lgrid->locks[i]);
}
