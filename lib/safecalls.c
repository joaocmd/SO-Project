void safe_dup2(int oldf, int newfd) {
    if ((dup2(oldfd, newfd)) < 0) {
        if (errno == EINTR) {
            safe_dup2(oldfd, newfd);
        }
    } else {
        perror("Error duplicating file descriptor.\n");
        exit(EXIT_FAILURE);
    }
}
