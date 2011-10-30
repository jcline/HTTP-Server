#include "include.h"

extern int errno;

static const char * const fourzerozero= "HTTP/1.0 400 Bad Request\x0d\x0a";
static const char * const fourzerofour= "HTTP/1.0 404 Not Found\x0d\x0a";
static const char * const fivezeroone= "HTTP/1.0 501 Not Implemented\x0d\x0a";
static const char * const twozerozero= "HTTP/1.0 200 OK\nContent-Type: text/plain\x0d\x0a";

void * pt_thread(void* args) {
	size_t len400 = strlen(fourzerozero);
	size_t len404 = strlen(fourzerofour);
	size_t len501 = strlen(fivezeroone);
	size_t len200 = strlen(twozerozero);

	const char * endtrans = "\x0d\x0a", 
		* restrict wdel = " \t",
		* restrict sdel = ":/",
		* restrict ndel = "\0";
	char * tok = NULL; 
	char * restrict buffer, * restrict tmpbuffer, *ptr, *pptr;
	int c_socket, s_port, s_socket, filds[2], rv = 0;
	long int h_addr;
	size_t BUFFER_SIZE = 500;
	ssize_t rc = 0;
	struct addrinfo hints, *result = NULL;
	struct hostent* s_info;
	struct list_t * request_list;
	struct node_t* restrict val;
	struct pt_args_t * params;
	struct sockaddr_in s_addr;

	buffer = (char *) malloc(sizeof(char)*BUFFER_SIZE),
	tmpbuffer = (char *) malloc(sizeof(char)*BUFFER_SIZE),

	params = (struct pt_args_t *) args;
	request_list = params->request_list;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM; 

	if(pipe(filds) < 0) {
		perror("pipe failed");
		return 0;
	}
	printf("filds: %d,%d\n", filds[0], filds[1]);

	while(1) {
		val = pop_front_n(request_list);

		if(!val)
			continue;

		c_socket = val->misc;

#ifndef NDEBUG 
		printf("%d: ", c_socket);
		fflush(stdout);
#endif


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
			s_port = atoi(ptr);
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
			strcat(tmpbuffer, ptr);

		}

		{
			struct addrinfo *i;
			for(i = result; i != NULL; i = i->ai_next) {
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

#ifndef NDEBUG
		printf("s: %d ", s_socket);
		fflush(stdout);
#endif

		rc = s_data(s_socket, tmpbuffer, rc);
#ifndef NDEBUG
			printf("req: %d ", rc);
			fflush(stdout);
#endif

		rc = sp_control(filds, c_socket, s_socket, 0);
#ifndef NDEBUG
			printf("send: %d ", rc);
			fflush(stdout);
#endif

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
		close(c_socket);
		close(s_socket);
		if(result)
			freeaddrinfo(result);
		result = NULL;
	}

	return NULL;
}



