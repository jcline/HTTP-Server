#include "include.h"

extern int errno;

static const char * const fourzerozero= "HTTP/1.0 400 Bad Request\x0d\x0a";
static const char * const fourzerofour= "HTTP/1.0 404 Not Found\x0d\x0a";
static const char * const fivezeroone= "HTTP/1.0 501 Not Implemented\x0d\x0a";
static const char * const twozerozero= "HTTP/1.0 200 OK\nContent-Type: text/plain\nContent-Length: ";

static int stop = 0;

void stop_thread() {
	stop = 1;
}

void * st_thread(void* args) {
	size_t len400 = strlen(fourzerozero);
	size_t len404 = strlen(fourzerofour);
	size_t len501 = strlen(fivezeroone);
	size_t len200 = strlen(twozerozero);
	size_t BUFFER_SIZE = 500;
	size_t TMPBUFFER_SIZE = 500;
	assert(args);

	struct st_args_t * params = (struct st_args_t *) args;

	struct list_t * request_list = params->request_list;

	assert(params->request_list);

	int file = 0, filds[2], local = 0, use_shared=params->use_shared, id = params->id, pass=0, req_id = 0;
	FILE* fptr = NULL;
	struct node_t* restrict val;
	struct stat statinfo;
	ssize_t sz = 0;
	int c_socket, rc;
	char* ptr = NULL, *endtrans = "\x0d\x0a",
		*buffer = (char *) malloc(sizeof(char)*BUFFER_SIZE),
		*tmpbuffer = (char *) malloc(sizeof(char)*BUFFER_SIZE),
		*tok = NULL, *del = " ";
	memset(buffer, 0, BUFFER_SIZE);
	memset(tmpbuffer, 0, BUFFER_SIZE);
	struct shm_thread_t * shared = NULL, ** t_t = params->share;

	if(pipe(filds) < 0) {
		perror("pipe failed");
		return 0;
	}

	struct sigaction sac;
	sac.sa_handler = stop_thread;
	sigaction(SIGUSR1, &sac, NULL);

	while(!stop) { 
		val = pop_front_n(request_list);
		if(!val) {
			continue;
		}
		local = 0;

		c_socket = val->misc;

#ifndef NDEBUG 
		printf("%d: ", c_socket);
		fflush(stdout);
#endif

		memset(buffer, 0, BUFFER_SIZE);
		// Read until theres nothing left
		rc = r_data_tv(c_socket, &buffer, (size_t*) &BUFFER_SIZE, endtrans, 2, 1, NULL);
#ifndef NDEBUG 
		printf("read: %d %s ", rc, buffer);
		fflush(stdout);
#endif
		if(rc == -1) {
			goto close;
		}
		ptr = buffer + rc;
		ptr = '\0';

		// Figure out what we should look up
		{
			if(use_shared) {
				if(BUFFER_SIZE > TMPBUFFER_SIZE) {
					tmpbuffer = realloc(tmpbuffer, BUFFER_SIZE);
					TMPBUFFER_SIZE=BUFFER_SIZE;
				}
				memmove(tmpbuffer, buffer, TMPBUFFER_SIZE);
			}
			ptr = strtok_r(buffer, del, &tok);
			if(!ptr) // there was no message we can work with
				goto fo0;
			// We just want the GET and the path
			if(strcmp(ptr, "GET")) {
				if(strcmp(ptr, "LOCAL_GET"))
					goto foo;
				else
					local = 1;
			}
			
			if(local) {
				ptr = strtok_r(NULL, del, &tok);
				if(!ptr)
					goto fof;

				req_id = atoi(ptr);
				assert(req_id >= 0 && req_id < MAX_SERVE_THREADS);
				if(req_id != id) {
				}
			}
			ptr = strtok_r(NULL, del, &tok);
			if(!ptr)
				goto fof;
			if(*ptr != '/')
				goto fof;
		}
#ifndef NDEBUG 
		printf("file: %s, ", ptr);
		fflush(stdout);
#endif

		// Drop the first character, should always be '/'
		++ptr;
		sz = strlen(ptr);
		if(!sz)
			goto fof;
		if(*ptr == '.')
			goto fof;
		++sz;

		if(stat(ptr, &statinfo) == -1) {
			perror("Could not stat");
			goto fof;
		}

		sz = statinfo.st_size;

		assert(local == 0 || local == 1);
		if(use_shared && local) {
			shared = t_t[req_id];
			pthread_mutex_lock(&shared->lock);
			fptr = fopen(ptr, "r");
			if(!fptr)
				goto foo;

			shared->size = 0;
			shared->done = 0;
			memmove(shared->data, twozerozero, len200);
			int rt = sprintf(shared->data+len200, "%d%s", sz, endtrans);
			shared->size = len200+rt;
#ifndef NDEBUG
			printf("header: %d ", shared->size);
			fflush(stdout);
#endif
		}
		else {
			if((file = open(ptr, O_RDONLY)) == -1) {
				perror("");
				goto fof;
			}
			rc = s_data( c_socket, twozerozero, len200); 
			int rt = sprintf(buffer, "%d%s%s", sz, endtrans, endtrans);
			rc += s_data(c_socket, buffer, rt);
			if( rc < 0 ) {
				goto close;
			}
#ifndef NDEBUG 
			printf("header: %d ", rc);
			fflush(stdout);
#endif
		}

		if(use_shared && local) {
			size_t s = (sz >= 1024*1024-10) ? (1024*1024)-10 : sz;
			assert(fptr);
			shared->size += fread(shared->data + shared->size, sizeof(char), s, fptr);

			shared->done = 1;
			shared->safe = 1;
			pthread_mutex_unlock(&shared->lock);
			pthread_cond_signal(&(shared->sig));
			fclose(fptr);
#ifndef NDEBUG
			printf("data: %d ", s);
			fflush(stdout);
#endif
		}
		else {
			rc = sp_control( filds, c_socket, file, sz); 
			if( rc < 0 ) {
				goto close;
			}
#ifndef NDEBUG 
			printf("data: %d ", rc);
			fflush(stdout);
#endif
			close(file);
		}

		goto close;

fo0:
		if(use_shared && local) {
			pthread_mutex_lock(&shared->lock);
			memmove(shared->data, fourzerozero, len400);
			shared->size = len400;
			shared->safe = 1;
			shared->done = 1;
			pthread_mutex_unlock(&shared->lock);
			pthread_cond_signal(&shared->sig);
#ifndef NDEBUG 
			printf("400: %d ", len400);
			fflush(stdout);
#endif
		}
		else {
			rc = s_data(c_socket, fourzerozero, len400);
#ifndef NDEBUG 
			printf("400: %d ", rc);
			fflush(stdout);
#endif
		}
		goto close;

fof:
		if(use_shared && local) {
			pthread_mutex_lock(&shared->lock);
			memmove(shared->data, fourzerofour, len404);
			shared->size = len404;
			shared->safe = 1;
			shared->done = 1;
			pthread_mutex_unlock(&shared->lock);
			pthread_cond_signal(&shared->sig);
#ifndef NDEBUG 
			printf("404: %d ", len404);
			fflush(stdout);
#endif
		}
		else {
			rc = s_data(c_socket, fourzerofour, len404);
#ifndef NDEBUG 
			printf("404: %d ", rc);
			fflush(stdout);
#endif
		}
		goto close;

foo:
		if(use_shared && local) {
			pthread_mutex_lock(&shared->lock);
			memmove(shared->data, fivezeroone, len501);
			shared->size = len501;
			shared->safe = 1;
			shared->done = 1;
			pthread_mutex_unlock(&shared->lock);
			pthread_cond_signal(&shared->sig);
#ifndef NDEBUG 
			printf("501: %d ", len501);
			fflush(stdout);
#endif
		}
		else {
			rc = s_data(c_socket, fivezeroone, len501);
#ifndef NDEBUG 
			printf("501: %d ", rc);
			fflush(stdout);
#endif
		}
		goto close;

close:
		free(val->data);
		free(val);

		if( close(c_socket) == -1 ) {
#ifndef NDEBUG
			fprintf(stderr,"Could not close c_socket: %d, %s %d\n", errno, strerror(errno), pass);
			fflush(stdout);
#endif
		}
	}

	free(buffer);
	free(tmpbuffer);

	if(use_shared) {
		shared = t_t[id];
		int proxy = shared->proxy;
		printf("proxy: %d\n", proxy);
		shared->web = 0;
		shared_end(shared);
		if(!proxy)
			shmctl(params->shmid, IPC_RMID, 0);
	}
	printf("Thread shutdown\n");

	return NULL;
}


