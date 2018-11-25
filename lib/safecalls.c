void safe_mkfifo(const char *path, mode_t mode){
	if ((mkfifo(path, mode)) < 0) {
		if (errno == EINTR) {
			return safe_mkfifo(path, mode);
		} else {
			perror("AdvShell: Couldn't create server pipe.\n");
			exit(EXIT_FAILURE);
		}
	}
}


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


void safe_unlink(const char *pathname) {
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
			perror("Error reading from the ??.\n");
			exit(EXIT_FAILURE);
		}
	}
	return bread;
}


void safe_write(int fildes, const void *buf, size_t nbyte){
	if ((write(fildes, buf, nbyte)) < 0) {
        if (errno == EINTR) {
            safe_write(fildes, buf, nbyte);
        }
    } else {
        perror("Error writing from the ??.\n");
        exit(EXIT_FAILURE);
    }
}


int safe_open(const char *path, int oflag) {
	int fd = open(path, oflag);
	if (fd < 0){
		if (errno == EINTR) {
			return safe_open(path, oflag);
		} else {
			perror("AdvShell: Error opening client pipe.\n");
			exit(EXIT_FAILURE);
		}
	}
	return fd;
}


void safe_close(int fildes){
	if ((close(fildes)) < 0) {
        if (errno == EINTR) {
            safe_close(fildes);
        }
    } else {
        perror("Error closing file.\n");
        exit(EXIT_FAILURE);
    }
}








