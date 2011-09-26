#ifndef RTHREADS_H
#define RTHREADS_H

#include "include.h"

struct rt_args_t {
	struct list_t *filename_list,
		*file_list;
	int done;
};

void *rt_thread(void* args);

#endif
