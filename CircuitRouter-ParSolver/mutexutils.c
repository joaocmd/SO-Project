#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include "mutexutils.h"
#include "lib/types.h"

/* =============================================================================
 * mutils_init
 * =============================================================================
 */
void mutils_init(pthread_mutex_t* mutex) {
    if (pthread_mutex_init(mutex, NULL)) {
        perror("mutils_init: could not initialize mutex. Aborting.");
        exit(1);
    }
}

/* =============================================================================
 * mutils_destroy
 * =============================================================================
 */
void mutils_destroy(pthread_mutex_t* mutex) {
    if (pthread_mutex_destroy(mutex)) {
        perror("mutils_destroy: could not destroy mutex. Resuming.");
    }
}


/* =============================================================================
 * mutils_locks
 * =============================================================================
 */
void mutils_lock(pthread_mutex_t* mutex) {
    if (pthread_mutex_lock(mutex)) {
        perror("mutils_lock: could not lock mutex. Aborting.");
        exit(1);
    }
}


/* =============================================================================
 * mutils_trylock
 * - if succeeds, returns whether the mutex got locked (true, success) or if it
 *   was already locked (false, failed);
 * =============================================================================
 */
bool_t mutils_trylock(pthread_mutex_t* mutex) {
    int err = pthread_mutex_trylock(mutex);
    if (err == 0 || err == EBUSY) {
        printf("%i\n", err);
        return !err;
    }
    else {
        pthread_mutex_trylock(mutex);
        perror("mutils_trylock: could not trylock mutex. Aborting.");
        exit(1);
    } 
}


/* =============================================================================
 * mutils_unlock
 * =============================================================================
 */
void mutils_unlock(pthread_mutex_t* mutex) {
    if (pthread_mutex_unlock(mutex)) {
        perror("mutils_unlock: could not unlock mutex. Aborting.");
        exit(1);
    }
}
