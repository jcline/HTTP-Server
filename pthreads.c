#include "include.h"

extern int errno;
static int stop = 0;

static const char * const fourzerozero= "HTTP/1.0 400 Bad Request\x0d\x0a";
static const char * const fourzerofour= "HTTP/1.0 404 Not Found\x0d\x0a";
static const char * const fivezeroone= "HTTP/1.0 501 Not Implemented\x0d\x0a";
static const char * const twozerozero= "HTTP/1.0 200 OK\nContent-Type: text/plain\x0d\x0a";

void stop_thread() {
	stop = 1;
}

void * pt_thread(void* args) {
	size_t len400 = strlen(fourzerozero);
	size_t len404 = strlen(fourzerofour);
	size_t len501 = strlen(fivezeroone);

	const char * endtrans = "\x0d\x0a", 
		* restrict wdel = " \t",
		* restrict sdel = ":/";
	char * tok = NULL; 
	char * restrict buffer, * restrict tmpbuffer, *ptr, *pptr;
	int c_socket, s_socket = 0, filds[2], rv = 0, use_shared = 0, local = 0, id = 0;
	size_t BUFFER_SIZE = 500;
	size_t TMPBUFFER_SIZE = 500;
	ssize_t rc = 0;
	struct addrinfo hints, *result = NULL, *me = NULL;
	struct list_t * request_list;
	struct node_t* restrict val;
	struct pt_args_t * params;
	struct shm_thread_t * shared;

	buffer = (char *) malloc(sizeof(char)*BUFFER_SIZE),
	tmpbuffer = (char *) malloc(sizeof(char)*BUFFER_SIZE),

	memset(buffer, 0, BUFFER_SIZE);
	memset(tmpbuffer, 0, TMPBUFFER_SIZE);

	params = (struct pt_args_t *) args;
	request_list = params->request_list;
	use_shared = params->use_shared;
	shared = params->share;
	id = params->id;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM; 

	if(pipe(filds) < 0) {
		perror("pipe failed");
		return 0;
	}

	struct sigaction sac;
	sac.sa_handler = stop_thread;
	sigaction(SIGUSR1, &sac, NULL);

	while( (rv = getaddrinfo("localhost", NULL, &hints, &me)) != 0) {
		fprintf(stderr,"getaddrinfo failure: %s", gai_strerror(rv));
	}

	while(!stop) {
		val = pop_front_n(request_list);

		if(!val)
			continue;

		c_socket = val->misc;

#ifndef NDEBUG 
		printf("%d: ", c_socket);
		fflush(stdout);
#endif

		memset(buffer, 0, BUFFER_SIZE);

		// Read until theres nothing left
		rc = r_data_tv(c_socket, &buffer, (size_t*) &BUFFER_SIZE, endtrans, 2, 1, NULL);
#ifndef NDEBUG 
		printf("read: %d ", rc);
#endif
		if(rc == -1) {
			goto close;
		}
		ptr = buffer + rc;
		ptr = '\0';

		// Make sure this is a GET request
		{
			ptr = strtok_r(buffer, wdel, &tok);
			if(!ptr) // there was no message we can work with
				goto fo0;
			if(strcmp(ptr, "GET"))
				goto foo;
			tmpbuffer[0] = '\0';
			strcat(tmpbuffer, ptr);
			
			ptr = strtok_r(NULL, sdel, &tok);
			if(!ptr)
				goto fof;
			if(strcmp(ptr, "http"))
				goto fof;

			ptr = strtok_r(NULL, sdel, &tok);
			if(!ptr)
				goto fof;

			pptr = strtok_r(NULL, sdel, &tok);
			if(!ptr)
				goto fof;

			if( (rv = getaddrinfo(ptr, pptr, &hints, &result)) != 0) {
				fprintf(stderr,"getaddrinfo failure: %s", gai_strerror(rv));
				goto fof;
			}
			assert(result);
#ifndef NDEBUG
			printf("CON: %s:%s ", ptr, pptr);
			fflush(stdout);
#endif

			ptr = strtok_r(NULL, "\0", &tok);
			if(!ptr)
				goto fof;
			strcat(tmpbuffer, " /");
			strncat(tmpbuffer, ptr, TMPBUFFER_SIZE-strlen(tmpbuffer)-1);

		}

		{
			struct addrinfo *i, *j;
			// Try to find a valid entry we can connect to from getaddrinfo call
			for(i = result; i != NULL; i = i->ai_next) {
				// Detect local connections
				if(use_shared) {
					local=0;
					for(j = me; j != NULL; j = j->ai_next) {
						switch(i->ai_family) {
							case AF_INET:
								if(((struct sockaddr_in *) i->ai_addr)->sin_addr.s_addr == 
								((struct sockaddr_in *) j->ai_addr)->sin_addr.s_addr)
									local = 1;
								goto cont;
								break;
							// Untested ipv6 support, theoretically correct
							case AF_INET6:
								if(!memcmp( ((struct sockaddr_in6 *) i->ai_addr)->sin6_addr.s6_addr,
								((struct sockaddr_in6 *) j->ai_addr)->sin6_addr.s6_addr, 16))
									local = 1;
								goto cont;
								break;
						}
					}
				}
cont:
				// Try to connect to entry for getaddrinfo lookup
				if( (s_socket = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1) {
					continue;
				}

				if( connect(s_socket, i->ai_addr, i->ai_addrlen) == -1) {
					close(s_socket);
					continue;
				}
				break;
			}

			if(!i)
				goto fof;
		}
		if(use_shared && local) {
			int i, j, rt;
			rt = sprintf(buffer, "%d ", id);
			printf("id: %d ", id);
			for(i = strlen(tmpbuffer)+rt+1, j=i-rt;
					j >= rt; --i, --j) {
				tmpbuffer[i] = tmpbuffer[j];
			}
			for(i = 4, j = 0; j < rt; ++i, ++j) {
				tmpbuffer[i] = buffer[j];
			}
			for(i = strlen(tmpbuffer)+strlen("LOCAL_")+1, j=i-strlen("LOCAL_");
					i >= strlen("LOCAL_"); --i, --j) {
				tmpbuffer[i] = tmpbuffer[j];
			}
			for(i = 0; i < strlen("LOCAL_"); ++i) {
				tmpbuffer[i] = "LOCAL_"[i];
			}
			rc = strlen(tmpbuffer);
		}

#ifndef NDEBUG
		printf("s: %d ", s_socket);
		fflush(stdout);
#endif

		rc = s_data(s_socket, tmpbuffer, rc);
#ifndef NDEBUG
		printf("req: %d ", rc);
		fflush(stdout);
#endif

		if(use_shared && local) {
			/*pthread_mutex_lock(&shared->lock);
			while(!shared->safe)
				pthread_cond_wait(&shared->sig, &shared->lock);
				*/

			while(!stop) {
				while(!shared->safe && !stop);

				rc = s_data(c_socket, shared->data, shared->size);

				shared->safe = 0;
				shared->size = 0;
				//pthread_mutex_unlock(&shared->lock);
#ifndef NDEBUG
				printf("send: %d ", rc);
				fflush(stdout);
#endif
				if(shared->done) {
					shared->done = 0;
					break;
				}
			}
		}
		else {
			rc = sp_control(filds, c_socket, s_socket, 0);
#ifndef NDEBUG
			printf("send: %d ", rc);
			fflush(stdout);
#endif
		}
		goto close;

fo0:
		rc = s_data(c_socket, fourzerozero, len400);
#ifndef NDEBUG 
		printf("400: %d ", rc);
		fflush(stdout);
#endif
		goto close;

fof:
		rc = s_data(c_socket, fourzerofour, len404);
#ifndef NDEBUG 
		printf("404: %d ", rc);
		fflush(stdout);
#endif
		goto close;

foo:
		rc = s_data(c_socket, fivezeroone, len501);
#ifndef NDEBUG 
		printf("501: %d ", rc);
		fflush(stdout);
#endif
		goto close;

close:
#ifndef NDEBUG 
		printf("closing");
		fflush(stdout);
#endif
		close(c_socket);
		close(s_socket);
		if(result)
			freeaddrinfo(result);
		result = NULL;
		free(val->data);
	}

	if(use_shared) {
		int web = shared->web;
		printf("web: %d\n", web);
		shared->proxy = 0;
		shared_end(shared);
		if(!web)
			shmctl(params->shmid, IPC_RMID, 0);
	}

	freeaddrinfo(me);
	free(buffer);
	free(tmpbuffer);

	printf("Thread shutdown\n");

	return NULL;
}

