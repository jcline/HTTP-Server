#include "include.h"

extern int errno;

static const char * const fourzerozero= "HTTP/1.0 400 Bad Request\x0d\x0a";
static const char * const fourzerofour= "HTTP/1.0 404 Not Found\x0d\x0a";
static const char * const fivezeroone= "HTTP/1.0 501 Not Implemented\x0d\x0a";
static const char * const twozerozero= "HTTP/1.0 200 OK\nContent-Type: text/plain\x0d\x0a";

void * st_thread(void* args) {
	size_t len400 = strlen(fourzerozero);
	size_t len404 = strlen(fourzerofour);
	size_t len501 = strlen(fivezeroone);
	size_t len200 = strlen(twozerozero);
	size_t BUFFER_SIZE = 500;
	assert(args);

	struct st_args_t * params = (struct st_args_t *) args;

	struct list_t * request_list = params->request_list,
								* file_list = params->file_list;

	assert(params->request_list);
	assert(params->file_list);

	struct node_t* restrict val, * restrict file;
	ssize_t i = 0, sz = 0;
	int c_socket, rc, rs;
	char* ptr = NULL, *endtrans = "\n\r\x0d\x0a",
		*buffer = (char *) malloc(sizeof(char)*BUFFER_SIZE),
		*tok = NULL, *del = " ";
	for(i = 0; i < BUFFER_SIZE; ++i) {
		buffer[i] = '\0';
	}
	while(1) { 
		val = pop_front_n(request_list);

		if(!val)
			continue;

		c_socket = val->misc;

#ifndef NDEBUG 
		printf("%d: ", c_socket);
#endif

		ptr = buffer;
		rs = BUFFER_SIZE;
		rc = 0;
		do {
			rs -= rc;
			if(rs <= 0) {
				rs = BUFFER_SIZE + 1000;
				ptr = (char *) realloc(buffer, rs);
				ptr += BUFFER_SIZE;
				BUFFER_SIZE = rs;
#ifndef NDEBUG 
				printf("buffer resize: %lu ", BUFFER_SIZE);
#endif
			}

			// Read until theres nothing left
			rc = read(c_socket, ptr, rs);
#ifndef NDEBUG 
			printf("read: %d ", rc);
#endif
			if(rc == -1) {
#ifndef NDEBUG
				fprintf(stderr,"Read error: %d, %s\n", errno, strerror(errno));
#endif
				goto fof;
			}
			ptr += rc;
			for(i = 0; i < ptr - buffer; ++i) {
				if(buffer[i] == '\x0d' || buffer[i] == '\x0a')
					goto rdone;
			}
		} while(rc != 0);
rdone:
		++ptr;
		ptr = '\0';

		// Figure out what we should look up
		{
			ptr = strtok_r(buffer, del, &tok);
			if(!ptr) // there was no message we can work with
				goto fo0;
			// We just want the GET and the path
			if(strncmp(ptr, "GET", 3))
				goto foo;
			
			ptr = strtok_r(NULL, del, &tok);
			if(!ptr)
				goto fof;
			if(ptr[0] != '/')
				goto fof;

			// we want to find the length of the path
			rs = strtok_r(NULL, del, &tok) - ptr;
		}
#ifndef NDEBUG 
			printf("file: %s ", ptr);
#endif

		// Drop the first character, should always be '/'
		++ptr;
		sz = strlen(ptr);
		if(!sz)
			goto fof;
		++sz;

		file = getval_n_l(file_list, ptr, sz);
		if(!file)
			goto fof;

		ptr = (char * restrict) file->data;
		assert(ptr);
		sz = file->size;

		rc = write( c_socket, twozerozero, len200); 
		if( rc < 0 ) {
#ifndef NDEBUG
			fprintf(stderr,"Could not write: %d, %s\n", errno, strerror(errno));
#endif
			goto close;
		}
#ifndef NDEBUG 
		printf("header: %d ", rc);
#endif

		for(i = 0; i < sz; ) {
		  rc = write( c_socket, &ptr[i], sz-i); 
		  if( rc < 0 ) {
#ifndef NDEBUG
		    fprintf(stderr,"Could not write: %d, %s\n", errno, strerror(errno));
#endif
				goto close;
		  }
#ifndef NDEBUG 
			printf("data: %d ", rc);
#endif
		  i += rc;
		}

		rc = write( c_socket, endtrans, 4);
#ifndef NDEBUG 
			printf("end: %d ", rc);
#endif
		if( rc < 0 )
#ifndef NDEBUG
			fprintf(stderr,"Could not write: %d, %s\n", errno, strerror(errno));
#endif
		goto close;

fo0:
		write(c_socket, fourzerozero, len400);
#ifndef NDEBUG 
			printf("400: %d ", rc);
#endif
		goto close;

fof:
		write(c_socket, fourzerofour, len404);
#ifndef NDEBUG 
			printf("404: %d ", rc);
#endif
		goto close;

foo:
		write(c_socket, fivezeroone, len501);
#ifndef NDEBUG 
			printf("501: %d ", rc);
#endif
		goto close;

close:
#ifndef NDEBUG 
			printf("\n");
#endif
		free(val->data);
		free(val);

		if( close(c_socket) == -1 ) {
#ifndef NDEBUG
		  fprintf(stderr,"Could not close c_socket: %d, %s\n", errno, strerror(errno));
#endif
		}

	}

	free(buffer);

	return NULL;
}


