#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <assert.h>

#define MEGABYTE 1048576

/* Safe printf into a fixed-size buffer */
#define bprintf(buf, fmt, ...)                      \
    do {                                \
        assert(snprintf(buf, sizeof buf, fmt, __VA_ARGS__)  \
            < (long int)(sizeof buf));                  \
    } while (0)

struct log_entry {
	pid_t pid;
	time_t ts;
	char msg[MEGABYTE];
};

struct shm_log {
	unsigned size;
	struct log_entry *head;
};

struct shm_log*
shmlog_create(int s, char *fn);
