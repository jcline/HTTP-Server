#include "include.h"

extern int errno;

static const char * const restrict fourzerofour= "HTTP/1.0 404 Not Found\n\r";
static const char * const restrict fivezeroone= "HTTP/1.0 501 Not Implemented\n\r";
static const char * const restrict twozerozero= "HTTP/1.0 200 OK\n";
static const char * const restrict get = "GET /";
static const char * const restrict htv = " HTTP/1.0\x0d\x0a";

void * ct_thread(void* args) {
	char * buffer, * ptr;
	int c, i, j, done, num_files, s_port, s_socket, rc;
	long int r, h_addr;

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

	buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE);
	for(i = 0; i < BUFFER_SIZE; ++i) {
		buffer[i] = '\0';
	}

	done = params->done;
	file_list = params->file_list;
	num_files = file_list->size;
	rc = 0;
	s_info = params->s_info;
	s_port = params->s_port;
	stats = params->stats;
	stats->rtimes = (long long int *) malloc(sizeof(long long int) * done);
	stats->ftimes = (long long int *) malloc(sizeof(long long int) * done);
	stats->dtimes = (long long int *) malloc(sizeof(long long int) * done);
	assert(stats->rtimes);
	assert(stats->ftimes);
	assert(stats->dtimes);

	for(i = 0; i < done; ++i) {
		stats->rtimes[i] = 0;
		stats->ftimes[i] = 0;
		stats->dtimes[i] = 0;
	}

	memcpy(&h_addr, s_info->h_addr, s_info->h_length);

	s_addr.sin_addr.s_addr = h_addr;
	s_addr.sin_port=htons(s_port);
	s_addr.sin_family=AF_INET;

	s_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	srand48_r(pthread_self(), &state);

	gettimeofday(&fs1, NULL);
	for(i = 0; i < done; ++i) {
		gettimeofday(&ts1, NULL);
		s_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		lrand48_r(&state, &r);
		r = (int)(r % 100);
		r *= 10000;
		usleep(r);

		gettimeofday(&rs1, NULL);
		if(connect(s_socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1) {
			perror("Could not connect to server");
			close(s_socket);
			continue;
		}

		lrand48_r(&state, &r);
		r = (int)(r % num_files);

		file = getval_n(file_list, r);

		write(s_socket, get, lenget);
		ptr = (char *) file->data;
		sz = file->size;
		write(s_socket, ptr, sz);
		write(s_socket, htv, lenhtv);

		c = 0;
		j = 0;
		ptr = buffer;
		sz = BUFFER_SIZE;
		gettimeofday(&ds, NULL);
		do {
			rc = read(s_socket, ptr, sz);
			if(!c)
				gettimeofday(&rs2, NULL);
			c += rc;
			if(rc == -1) {
				break;
			}
			ptr += rc;
			for(j = 0; j < ptr - buffer; ++j) {
				if(buffer[j] == '\x0d' || buffer[j] == '\x0a')
					goto rdone;
			}
			if(sz < rc)
				break;
			sz -= rc;
		} while(rc != 0);

rdone:
		gettimeofday(&ts2, NULL);
		ptr = '\0';
		if(c > 3) {
			for(j = 0; j < c; ++j) {
				if(buffer[j] == ' ') {
					++j;
					break;
				}
			}
			switch (buffer[j]) {
				case '2':
					++stats->OK;
					break;
				case '4':
					if(buffer[j+2] == '0')
						++stats->BAD;
					else
						++stats->FOUND;
					break;
				case '5':
					++stats->IMPL;
					break;
				default:
					break;
			}


		}

		close(s_socket);

		stats->rtimes[i] = (rs2.tv_sec - rs1.tv_sec) * 1000 * 1000;
		stats->rtimes[i] += (rs2.tv_usec - rs1.tv_usec);
		stats->ftimes[i] = (ts2.tv_sec - ts1.tv_sec) * 1000 * 1000;
		stats->ftimes[i] += (ts2.tv_usec - ts1.tv_usec);
		stats->dtimes[i] = (ts2.tv_sec - ds.tv_sec) * 1000 * 1000;
		stats->dtimes[i] += (ts2.tv_usec - ds.tv_usec);
	}
	gettimeofday(&fs2, NULL);
	stats->ttime = (fs2.tv_sec - fs1.tv_sec) * 1000 * 1000;
	stats->ttime += (fs2.tv_usec - fs1.tv_usec);

	free(args);
	free(buffer);


	return NULL;
}


