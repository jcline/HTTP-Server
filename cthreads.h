#ifndef CTHREADS_H
#define CTHREADS_H

#include "include.h"

struct ct_stats_t {
	unsigned long long int OK, BAD, FOUND, IMPL, tr;
	int ttime, *rtimes, *ftimes, *dtimes;
	double *rate;
};

struct ct_args_t {
	struct list_t * file_list;
	int done, s_port, p_port;
	struct sockaddr_in * s_addr;
	struct hostent* s_info;
	struct ct_stats_t * stats;
	char* s_name;
};

void * ct_thread(void* args);

#endif
