/*
 * Read shm log
 */

#include "shmlog.h"

int
main (int argc, char *argv[]) {
	int fd;
	struct log_entry *log_head;
	struct stat st;
	unsigned end;

	if (argc < 2) {
		fprintf(stderr,"Usage: %s <shm-logfile>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open %s: %s\n",
				argv[1], strerror(errno));
		exit(1);
	}
	if (fstat(fd, &st) < 0) {
		fprintf(stderr, "Could not fstat %s: %s\n",
				argv[1], strerror(errno));
		exit(1);
	}

	log_head = (void*)mmap(NULL, st.st_size,
						   PROT_READ, MAP_SHARED, fd, 0);

	if (log_head == MAP_FAILED) {
		fprintf(stderr, "Could not mmap log %s: %s\n",
			    argv[1], strerror(errno));
		exit(1);
	}

	end = (unsigned)log_head + st.st_size;
	do {
		printf("[%d] -- [%ld]: %s\n",log_head->pid, log_head->ts, log_head->msg);
	} while ((unsigned)log_head++ < end);

	exit(0);
}
