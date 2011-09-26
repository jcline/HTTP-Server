#ifndef STHREADS_H
#define STHREADS_H

#include "include.h"

struct st_args_t {
	struct list * file_list;
};

void * st_thread(void* args);

#endif
