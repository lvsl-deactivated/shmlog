#include "shmlog.h"

int
main (int argc, char *argv[]) {
	unsigned size;
	struct log_entry *m_head;
	struct log_entry *start;
	struct log_entry *end;
	struct shm_log *log_head;
	unsigned wc;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <shm-log-file> <size-in-mb>\n",
				argv[0]);
		exit(1);
	}

	log_head = shmlog_create(atoi(argv[2]), argv[1]);
	if (log_head == NULL) {
		fprintf(stderr, "Could not create shmlog %s with size %s\n",
				argv[1], argv[2]);
		exit(1);
	}

	m_head = log_head->head;
	size = log_head->size;

	/* test writing to memory */
	start = m_head;
	end = (struct log_entry*)((unsigned)m_head + size);
	wc = 0;
	while (1) {
		if (m_head >= end) {
			//printf("wrap: %d\n", wc++);
			m_head = start;
		};
	    m_head->pid = getpid();
		m_head->ts = time(NULL);
		memset(&m_head->msg, 0, sizeof m_head->msg);
		bprintf(m_head->msg, "%s", "Log initialized\n");
		m_head++;

		//printf("m_head: %u\n", (unsigned)m_head);
		//printf("start: %u\n", (unsigned)start);
		//printf("end: %u\n", (unsigned)end);
		//sleep(3);
	}
	/* end test */

	exit(0);
}
