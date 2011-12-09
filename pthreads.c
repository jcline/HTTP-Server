#include "include.h"
#include "jpeg.h"

extern int errno;

static const char * const fourzerozero= "HTTP/1.0 400 Bad Request\x0d\x0a";
static const char * const fourzerofour= "HTTP/1.0 404 Not Found\x0d\x0a";
static const char * const fivezeroone= "HTTP/1.0 501 Not Implemented\x0d\x0a";
static const char * const twozerozero= "HTTP/1.0 200 OK\nContent-Type: text/plain\x0d\x0a";

void * pt_thread(void* args) {
	size_t len400 = strlen(fourzerozero);
	size_t len404 = strlen(fourzerofour);
	size_t len501 = strlen(fivezeroone);

	const char * endtrans = "\x0d\x0a", 
		* restrict wdel = " \t",
		* restrict sdel = ":/";
	char * host = NULL;
	char * tok = NULL; 
	char * restrict buffer, * restrict tmpbuffer, *ptr, *pptr;
	int c_socket, s_socket = 0, filds[2], rv = 0, jpeg = 0;
	size_t BUFFER_SIZE = 500;
	size_t TMPBUFFER_SIZE = 500;
	ssize_t rc = 0;

	CLIENT * client;

	struct addrinfo hints, *result = NULL;
	struct list_t * request_list;
	struct node_t* restrict val;
	struct pt_args_t * params;

	buffer = (char *) malloc(sizeof(char)*BUFFER_SIZE),
	tmpbuffer = (char *) malloc(sizeof(char)*TMPBUFFER_SIZE),

	params = (struct pt_args_t *) args;
	request_list = params->request_list;
	host = params->host;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM; 

	if(pipe(filds) < 0) {
		perror("pipe failed");
		return 0;
	}
	printf("filds: %d,%d\n", filds[0], filds[1]);

	while(1) {
		jpeg = 0;
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
		printf("read: %d, %s\n", rc, buffer);
#endif
		if(rc == -1) {
			goto close;
		}
		ptr = buffer + rc;
		ptr = '\0';

		// Make sure this is a GET request
		{
			if(BUFFER_SIZE > TMPBUFFER_SIZE) {
				TMPBUFFER_SIZE = BUFFER_SIZE;
				realloc(tmpbuffer, TMPBUFFER_SIZE);
			}
			memcpy(tmpbuffer, buffer, rc);
			ptr = strtok_r(buffer, wdel, &tok);
			if(!ptr) // there was no message we can work with
				goto fo0;

			if(strcmp(ptr, "GET"))
				goto foo;
			ptr = strtok_r(NULL, sdel, &tok);
			if(!ptr)
				goto fof;
			if(strcmp(ptr, "http"))
				goto fof;

			ptr = strtok_r(NULL, sdel, &tok);
			if(!ptr)
				goto fof;

			pptr = strtok_r(NULL, sdel, &tok);
			printf("%s\t%s\n", ptr, pptr);
			if(!ptr)
				goto fof;

			int noport = 1;
			if(isdigit(pptr[0])) {
				noport = 0;
				if( (rv = getaddrinfo(ptr, pptr, &hints, &result)) != 0) {
					fprintf(stderr,"getaddrinfo failure: %s", gai_strerror(rv));
					goto fof;
				}
			}
			else {
				if( (rv = getaddrinfo(ptr, "80", &hints, &result)) != 0) {
					fprintf(stderr,"getaddrinfo failure: %s", gai_strerror(rv));
					goto fof;
				}
			}
			assert(result);
#ifndef NDEBUG
			if(noport)
				printf("CON: %s:80 ", ptr);
			else
				printf("CON: %s:%s ", ptr, pptr);
			fflush(stdout);
#endif

		}

		ptr = strstr(tmpbuffer, "HTTP");
		ptr[7] = '0';
		printf("\ntmpbuffer: %s\n", tmpbuffer);
		ptr[8] = '\x0d';
		ptr[9] = '\x0a';
		ptr[10] = '\x0d';
		ptr[11] = '\x0a';
		ptr[12] = '\0';

		ptr -= 4;

		if(ptr[0] == 'j' || ptr[0] == 'J')
			if(ptr[1] == 'p' || ptr[1] == 'P')
				if(ptr[2] == 'g' || ptr[2] == 'G')
					jpeg = 1;

		{
			struct addrinfo *i;
			for(i = result; i != NULL; i = i->ai_next) {
				if( (s_socket = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1) {
#ifndef NDEBUG
					perror("socket");
#endif
					continue;
				}

				if( connect(s_socket, i->ai_addr, i->ai_addrlen) == -1) {
#ifndef NDEBUG
					perror("connect");
#endif
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

		rc = s_data(s_socket, tmpbuffer, strlen(tmpbuffer));
#ifndef NDEBUG
			printf("req: %d ", rc);
			fflush(stdout);
#endif

		if(jpeg) {
			if ((client = clnt_create(host, JPEG_SHRINK, JPEG_VERS, "tcp")) == NULL) {
				/*
				* can't establish connection with server
				*/
				clnt_pcreateerror(host);
				// Fail gracefully by just sending the full image
				rc = sp_control(filds, c_socket, s_socket, 0);
			}
			else {
				img_t i;
				memset(tmpbuffer, 0, TMPBUFFER_SIZE);
				rc = r_data_tv_c(s_socket, &tmpbuffer, &TMPBUFFER_SIZE, NULL);

				ptr = strstr(tmpbuffer, "\x0d\x0a\x0d\x0a");
				ptr += 4;
				i.data = ptr;
				i.size = rc - (ptr - tmpbuffer);
				printf("i.size: %d\n", i.size);
				img_t * ret = shrink_img_1(&i, client);
				if(!ret)
					goto foo;
				printf("ret.size: %d\n", ret.size);

				memcpy(ptr, ret->data, ret->size);
				ptr += ret->size;
				ptr[0] = '\0';

				s_data(c_socket, tmpbuffer, ptr - tmpbuffer);

			}

		}
		else {
			rc = sp_control(filds, c_socket, s_socket, 0);
		}
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



