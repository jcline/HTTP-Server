#ifndef PTHREADS_H
#define PTHREADS_H

#include "include.h"

struct pt_args_t {
	struct list_t * request_list;
	int done, s_port;
	char* host;
	struct sockaddr_in * s_addr;
	struct hostent* s_info;
};

void * pt_thread(void* args);

#endif
