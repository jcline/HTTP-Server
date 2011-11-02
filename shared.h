#ifndef SHARED_H
#define SHARED_H

#include "include.h"

struct request_t {
	int fd;
	char request[255];
};

struct shm_thread_t {
	volatile size_t size;
	volatile int shutdown, ack, aware;
	char data[16384];
	pthread_mutex_t lock;
	pthread_cond_t sig;
	key_t key;
};

struct shm_control_t {
	unsigned char head, tail;
	struct request_t data[255];
	int init;
};

int shared_get(key_t key, size_t size);
struct shm_thread_t * shared_mmap(int id, size_t size);
void shared_end(const void* shm);

#endif
