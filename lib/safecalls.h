#ifndef SAFECALLS_H
#define SAFECALLS_H


/* =============================================================================
 * Safe Calls
 *  This package provides simple functions to avoid code repetition. The
 *  available functions are listed below and all they do is call the respective
 *  function and check if it returned an error, if they do, the program is
 *  ended with status EXIT_FAILURE.
 *  Most of these calls might return an error because they were interrupted by
 *  a signal, setting errno no EINTR, if that's the case, the call is repeated.
 * =============================================================================
 */


void safe_mkfifo(const char *pathname, mode_t mode);


void safe_unlink(const char *pathname);


void safe_dup2(int oldfd, int newfd);


int safe_read(int fildes, void *buf, size_t nbyte);


void safe_write(int fildes, const void *buf, size_t nbyte);


int safe_open(const char *path, int oflag);

void safe_close(int fildes);

#endif
