//
//  scat.m
//  super cat
//  because regular cat starts at the beginning, and I need to leap a few tall buildings
//
//  Created by Ryan Petrich on 10-03-06.
//  Copyright Ryan Petrich 2010. All rights reserved.
//

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_SEEK 0x0fffffff

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s /file/to/read offset\n", argv[0]);
		return 1;
	}
	
	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Error opening: %i\n", errno);
		return 1;
	}
	
	struct stat stat_buf;	
	if (fstat(fd, &stat_buf) == -1) {
		fprintf(stderr, "Error in fstat: %i\n", errno);
		close(fd);
		return 1;
	}
	size_t block_size = stat_buf.st_blksize;

	long long offset = atoll(argv[2]);
	fprintf(stderr, "Using block size: %lld\nFile size: %lld\nStart offset: %lld\n", (long long)block_size, (long long)stat_buf.st_size, (long long)offset);
	while (offset > MAX_SEEK) {
		lseek(fd, MAX_SEEK, SEEK_CUR);
		offset -= MAX_SEEK;
	}
	lseek(fd, offset, SEEK_CUR);
	
	char *buffer = malloc(block_size);
	ssize_t bytes_read;
	long long total = 0;
	while(bytes_read = read(fd, buffer, block_size)) {
		if (bytes_read == -1) {
			fprintf(stderr, "Error reading: %i\n", errno);
			close(fd);
			return 1;
		}
		ssize_t pos_in_buffer = 0;
		do {
			ssize_t bytes_written = write(STDOUT_FILENO, &buffer[pos_in_buffer], bytes_read - pos_in_buffer);
			if (bytes_written == -1) {
				fprintf(stderr, "Error writing: %i\n", errno);
				close(fd);
				return 1;
			}
			pos_in_buffer += bytes_written;
		} while (pos_in_buffer != bytes_read);
		total += bytes_read;
		fprintf(stderr, "Written: %lld\r", total);
	}
	fprintf(stderr, "\n");
	close(fd);
	
	return 0;
}
