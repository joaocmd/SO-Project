void safe_dup2(int oldfd, int newfd) {
    if ((dup2(oldfd, newfd)) < 0) {
        if (errno == EINTR) {
            safe_dup2(oldfd, newfd);
        }
    } else {
        perror("Error duplicating file descriptor.\n");
        exit(EXIT_FAILURE);
    }
}


void safe_unlink(const char *pathname){
	if ((unlink(pathname)) < 0){
		perror("Error unlinking file.\n");
		exit(EXIT_FAILURE);
	}			
}

int safe_read(int fildes, void *buf, size_t nbyte){
	int bread = read(fidels,buf,nbyte);
	if (bread < 0){
		if (errno == EINTR) {
			return safe_read(fildes, buf, nbyte);
		} else{
			perror("Error reading from the ??\n");
			exit(EXIT_FAILURE);
		}
	}
	return bread;
}






