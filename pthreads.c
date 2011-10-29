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
		* restrict sdel = ":/";
	char * tok = NULL; 
	char * restrict buffer, *ptr;
	int c_socket, s_port, s_socket, filds[2];
	long int h_addr;
	size_t BUFFER_SIZE = 500;
	ssize_t rc = 0;
	struct hostent* s_info;
	struct list_t * request_list;
	struct node_t* restrict val;
	struct pt_args_t * params;
	struct sockaddr_in s_addr;

	buffer = (char *) malloc(sizeof(char)*BUFFER_SIZE),

	params = (struct pt_args_t *) args;
	request_list = params->request_list;

	if(pipe(filds) < 0) {
		perror("pipe failed");
		return 0;
	}

	while(1) {
		val = pop_front_n(request_list);

		if(!val)
			continue;

		c_socket = val->misc;

#ifndef NDEBUG 
		printf("%d: ", c_socket);
		fflush(stdout);
#endif

		/*s_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(connect(s_socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1) {
			perror("Could not connect to server");
			close(s_socket);
			// TODO: Error to client
			continue;
		}
		*/

		// Read until theres nothing left
		rc = r_data_tv(c_socket, &buffer, (size_t*) &BUFFER_SIZE, endtrans, 2, 1, NULL);
#ifndef NDEBUG 
		printf("read: %d %s ", rc, buffer);
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
			// We just want the GET and the path
			if(strcmp(ptr, "GET"))
				goto foo;
			
			ptr = strtok_r(NULL, sdel, &tok);
			if(!ptr)
				goto fof;
			if(strcmp(ptr, "http")) {
				goto fof;

			ptr = strtok_r(NULL, sdel, &tok);
			if(!ptr)
				goto fof;
			}
		}
#ifndef NDEBUG
		printf("CON: %s ", ptr);
		fflush(stdout);
#endif

		goto fof;
		sp_control(filds, c_socket, s_socket, 0);

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
	}

/*
	size_t len501 = strlen(fivezeroone);
	size_t len200 = strlen(twozerozero);
	assert(args);


	assert(params->request_list);

	int file = 0;
	struct stat statinfo;
	ssize_t sz = 0;
	int c_socket, rc;
	memset(buffer, 0, BUFFER_SIZE);

	while(1) { 
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

#ifndef NDEBUG 
		printf("\t%s ", ptr);
		fflush(stdout);
#endif

		//file = getval_n_l(file_list, ptr, sz);
		if((file = open(ptr, O_RDONLY)) == -1) {
			perror("");
			goto fof;
		}

		if(stat(ptr, &statinfo) == -1) {
			perror("Could not stat");
			goto fof;
		}
		sz = statinfo.st_size;

		rc = s_data( c_socket, twozerozero, len200); 
		if( rc < 0 ) {
			goto close;
		}
#ifndef NDEBUG 
		printf("header: %d ", rc);
		fflush(stdout);
#endif

		rc = sp_control( c_socket, file, sz); 
		if( rc < 0 ) {
			goto close;
		}
#ifndef NDEBUG 
		printf("data: %d ", rc);
		fflush(stdout);
#endif

		rc = s_data( c_socket, endtrans, 2);
#ifndef NDEBUG 
		printf("end: %d ", rc);
		fflush(stdout);
#endif
		goto close;


close:
#ifndef NDEBUG 
		printf("\n");
		fflush(stdout);
#endif
		//free(val->data);
		free(val);

		if( close(c_socket) == -1 ) {
#ifndef NDEBUG
		fprintf(stderr,"Could not close c_socket: %d, %s\n", errno, strerror(errno));
		fflush(stdout);
#endif
		}

	}

	free(buffer);

*/
	return NULL;
}

