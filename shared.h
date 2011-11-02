#ifndef SHARED_H
#define SHARED_H

#include "include.h"

struct request_t {
	int fd;
	char request[255];
};

struct shm_thread_t {
	volatile size_t size;
	char data[16384]
	pthread_mutex_t lock;
	pthread_cond_t sig;
};

struct shm_control_t {
	unsigned char head, tail;
	struct request_t data[255];
	int init;
};

void shared_end(const void* shm);

#endif
