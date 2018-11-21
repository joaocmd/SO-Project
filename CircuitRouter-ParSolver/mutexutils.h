#ifndef MUTILS_H
#define MUTILS_H

#include <pthread.h>
#include "lib/types.h"

/* =============================================================================
 * Mutex Utilities
 *  This package provides simple functions to avoid code repetition. By checking
 *  the return value of mutex related function calls. All the functions abort
 *  the program if the call fails, except for mutils_destroy, since there's no
 *  point in aborting the whole program just because the destroy failed,
 *  mostly because the mutexes are only destroyed after the program has
 *  sucessfully ran.
 * =============================================================================
 */

/* =============================================================================
 * mutils_init
 * =============================================================================
 */
 void mutils_init(pthread_mutex_t* mutex);

/* =============================================================================
 * mutils_destroy
 * =============================================================================
 */
 void mutils_destroy(pthread_mutex_t* mutex);

/* =============================================================================
 * mutils_lock
 * =============================================================================
 */
void mutils_lock(pthread_mutex_t* mutex);


/* =============================================================================
 * mutils_lock
 * =============================================================================
 */
bool_t mutils_trylock(pthread_mutex_t* mutex);


/* =============================================================================
 * mutils_unlock
 * =============================================================================
 */
void mutils_unlock(pthread_mutex_t* mutex);

#endif
