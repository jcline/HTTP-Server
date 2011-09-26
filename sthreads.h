#ifndef STHREADS_H
#define STHREADS_H

#include "include.h"

struct st_args_t {
	struct list_t * file_list,
		    * request_list;
	int done;
};

void * st_thread(void* args);

#endif
