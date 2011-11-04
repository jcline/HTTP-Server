#ifndef SHARED_H
#define SHARED_H

#include "include.h"

struct shm_thread_t {
	volatile pthread_mutex_t lock;
	volatile pthread_cond_t sig;
	key_t key;
	volatile size_t size;
	volatile int web, proxy, init, done, safe;
	volatile char data[1024*1024];
};

int shared_manage(void** restrict ptr, int * restrict id, key_t key, size_t size);
void shared_end(const void* shm);

#endif
