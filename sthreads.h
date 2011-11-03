#ifndef STHREADS_H
#define STHREADS_H

#include "include.h"

struct st_args_t {
	struct list_t * request_list;
	int done, use_shared, shmid;
	struct shm_thread_t * share;
};

void * st_thread(void* args);

#endif
