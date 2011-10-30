#include "include.h"

extern int errno;

static const char * const restrict fourzerofour= "HTTP/1.0 404 Not Found\x0d\x0a"; 
static const char * const restrict fivezeroone= "HTTP/1.0 501 Not Implemented\x0d\x0a"; 
static const char * const restrict twozerozero= "HTTP/1.0 200 OK\x0d\x0a"; 
static const char * const restrict get = "GET";
static const char * const restrict htv = " HTTP/1.0\x0d\x0a\x0d\x0a";

void * ct_thread(void* args) {
	char * buffer, * ptr, * s_name;
	int c, i, done, num_files, s_port, s_socket, rc, p_port;
	long int r, h_addr;
	long long int slept = 0, rb;

	size_t sz;
	size_t lenget = strlen(get);
	size_t lenhtv = strlen(htv);
	size_t BUFFER_SIZE = 5000;

	struct ct_args_t * restrict params = (struct ct_args_t *) args;
	struct ct_stats_t * restrict stats;
	struct drand48_data state;
	struct hostent* s_info;
	struct list_t * restrict file_list;
	struct node_t * restrict file;
	struct sockaddr_in s_addr;
	struct timeval rs1, rs2, ts1, ts2, fs1, fs2, ds;

	buffer = (char *) calloc(sizeof(char), BUFFER_SIZE);

	done = params->done;
	file_list = params->file_list;
	num_files = file_list->size;
	rc = 0;
	s_info = params->s_info;
	s_port = params->s_port;
	p_port = params->p_port;
	s_name = params->s_name;
	stats = params->stats;
	stats->rtimes = (int *) malloc(sizeof(int) * done);
	stats->ftimes = (int *) malloc(sizeof(int) * done);
	stats->dtimes = (int *) malloc(sizeof(int) * done);
	assert(stats->rtimes);
	assert(stats->ftimes);
	assert(stats->dtimes);

	for(i = 0; i < done; ++i) {
		stats->rtimes[i] = 0;
		stats->ftimes[i] = 0;
		stats->dtimes[i] = 0;
	}
	stats->tr = 0;

	memcpy(&h_addr, s_info->h_addr, s_info->h_length);

	s_addr.sin_addr.s_addr = h_addr;
	s_addr.sin_port=htons(s_port);
	s_addr.sin_family=AF_INET;

	srand48_r(pthread_self(), &state);

	gettimeofday(&fs1, NULL);
	for(i = 0; i < done; ++i) {
		gettimeofday(&ts1, NULL);
		s_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		lrand48_r(&state, &r);
		r = (int)(r % 100);
		r *= 10000;
		//usleep(r);
		rb = r;
		slept += rb;

		lrand48_r(&state, &r);
		r = (int)(r % num_files);

		file = getval_n(file_list, r);

		gettimeofday(&rs1, NULL);
		if(connect(s_socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1) {
			perror("Could not connect to server");
			close(s_socket);
			continue;
		}

		ptr = (char *) file->data;

		if(s_name)
			sz = sprintf(buffer, "%s http://%s:%d%s %s", get, s_name, p_port, ptr, htv);
		else
			sz = sprintf(buffer, "%s %s %s", get, ptr, htv);

		c = s_data(s_socket, buffer, sz);
#ifndef NDEBUG
		printf("%d ", c);
		fflush(stdout);
#endif

		c = 0;
		gettimeofday(&ds, NULL);

		c = r_data_tv(s_socket, &buffer, &BUFFER_SIZE, "\x0d\x0a", 2, 2, &rs2);
		stats->tr += c;
#ifndef NDEBUG
		printf("%d\n", c);
		fflush(stdout);
#endif

		gettimeofday(&ts2, NULL);
		if(c > 3) {
			ptr = strstr(buffer, "200");
			if(!ptr) {
				ptr = strstr(buffer, "401");
				if(ptr) {
					++stats->BAD;
					goto skip;
				}
			}
			else
				++stats->OK;

			if(!ptr) {
				ptr = strstr(buffer, "404");
				if(ptr) {
					++stats->FOUND;
					goto skip;
				}
			}
			if(!ptr) {
				ptr = strstr(buffer, "501");
				if(ptr) {
					++stats->IMPL;
					goto skip;
				}
			}
		}

skip:
		close(s_socket);

		stats->rtimes[i] = (rs2.tv_sec - rs1.tv_sec) * 1000 * 1000;
		stats->rtimes[i] += (rs2.tv_usec - rs1.tv_usec);
		stats->ftimes[i] = (ts2.tv_sec - ts1.tv_sec) * 1000 * 1000;
		stats->ftimes[i] += (ts2.tv_usec - ts1.tv_usec);
		//stats->ftimes[i] -= rb;
		stats->dtimes[i] = (ts2.tv_sec - ds.tv_sec) * 1000 * 1000;
		stats->dtimes[i] += (ts2.tv_usec - ds.tv_usec);
	}
	gettimeofday(&fs2, NULL);
	stats->ttime = (fs2.tv_sec - fs1.tv_sec) * 1000 * 1000;
	stats->ttime += (fs2.tv_usec - fs1.tv_usec);
	//stats->ttime -= slept;

	free(args);
	free(buffer);


	return NULL;
}


