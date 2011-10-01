#ifndef CTHREADS_H
#define CTHREADS_H

#include "include.h"

struct ct_stats_t {
	long int OK, BAD, FOUND, IMPL;
	long long int ttime, *rtimes, *ftimes, *dtimes;
};

struct ct_args_t {
	struct list_t * file_list;
	int done, s_port;
	struct sockaddr_in * s_addr;
	struct hostent* s_info;
	struct ct_stats_t * stats;
};

void * ct_thread(void* args);

#endif
