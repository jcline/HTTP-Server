#ifndef SHARED_H
#define SHARED_H

#include "include.h"

struct shm_thread_t {
	volatile size_t size;
	volatile int web, proxy, init;
	char data[16384];
	pthread_mutex_t lock;
	pthread_cond_t sig;
	key_t key;
};

int shared_manage(void** restrict ptr, int * restrict id, key_t key, size_t size);
void shared_end(const void* shm);

#endif
