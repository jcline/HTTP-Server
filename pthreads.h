#ifndef PTHREADS_H
#define PTHREADS_H

#include "include.h"

struct pt_args_t {
	struct list_t * request_list;
	int done;
};

void * pt_thread(void* args);

#endif
