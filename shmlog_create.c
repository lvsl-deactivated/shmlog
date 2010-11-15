/*
 * Create shared memory
 *
 * Create file
 * Fill it with zeroes
 * Truncate it to pagesize bound
 * Mmap it with MAP_SHARED
 * Mlock it into memory
 */

#include "shmlog.h"

struct shm_log*
shmlog_create(int s, char *fn) {
	int fd;
	int i;
	unsigned u;
	unsigned sz;
	long int ps;
	unsigned size;
	char buf[MEGABYTE];
	struct log_entry *m_head;
	struct shm_log *log_head;

	ps = sysconf(_SC_PAGESIZE);
	sz = s * MEGABYTE;
	size = sz + ps - 1;
	size &= ~(ps - 1);

	fd = open(fn, O_RDWR | O_CREAT | O_EXCL, 0644);
	if (fd < 0) {
		fprintf(stderr, "Could not create: %s: %s\n",
				fn, strerror(errno));
		return NULL;
	}

	memset(buf, 0, sizeof buf);

	for (u = 0; u < size; ) {
		i = write(fd, buf, sizeof buf);
		if (i <= 0) {
			fprintf(stderr, "Write to log error %s: %s\n",
				    fn, strerror(errno));
			return NULL;
		}
		u += i;
	}

	if (ftruncate(fd, (off_t)size) < 0) {
		fprintf(stderr, "Could not ftruncate %s: %s\n",
				fn, strerror(errno));
		return NULL;
	}

	m_head = (void*)mmap(NULL, size,
		PROT_READ | PROT_WRITE,
		MAP_SHARED,
		fd, 0);

	if (m_head == MAP_FAILED) {
		fprintf(stderr, "Could not mmap %s: %s\n",
				fn, strerror(errno));
		return NULL;
	}

	if (mlock((void*)m_head, size) < 0) {
		fprintf(stderr, "Could not lock memory: %s\n"
				"Do you have permission to call mlock()?\n",
				strerror(errno));
	}

	if ((log_head = (struct shm_log*)malloc(sizeof (struct shm_log))) == NULL) {
		fprintf(stderr, "Could not malloc for %s: %s\n", fn, strerror(errno));
		return NULL;
	}

	if (memset((void *)log_head, 0, sizeof (struct shm_log)) == NULL) {
		fprintf(stderr, "Could not memset memory for %s: %s\n",
				fn, strerror(errno));
		return NULL;
	}

	log_head->size = size;
	log_head->head = m_head;

	return log_head;
}
