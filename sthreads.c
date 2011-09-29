#include "include.h"

extern int errno;

static char* fourzerofour = "404: File not Found\n\r";

void * st_thread(void* args) {
	size_t BUFFER_SIZE = 500;
	assert(args);

	struct st_args_t * params = (struct st_args_t *) args;

	struct list_t * request_list = params->request_list,
								* file_list = params->file_list;

	assert(params->request_list);
	assert(params->file_list);

	struct node_t* restrict val, * restrict file;
	ssize_t i = 0, sz = 0;
	int c_socket, rc, rs, flags = 0;
	char* ptr = NULL, *endtrans = "\n\r\0",
		*buffer = (char *) malloc(sizeof(char)*BUFFER_SIZE),
		*tok = NULL, *del = " ";
	for(i = 0; i < BUFFER_SIZE; ++i) {
		buffer[i] = '\0';
	}
	while(1) { 
retry:
		val = pop_front_n(request_list);

		if(params->done)
			return NULL;

		if(!val)
			continue;

		c_socket = val->misc;

    if (-1 == (flags = fcntl(c_socket, F_GETFL, 0)))
        flags = 0;
    fcntl(c_socket, F_SETFL, flags | O_NONBLOCK);

		ptr = buffer;
		rs = BUFFER_SIZE;
		rc = 0;
		i = 0;
		do {
			rs -= rc;
			if(rs <= 0) {
				rs = BUFFER_SIZE + 1000;
				ptr = (char *) realloc(buffer, rs);
				ptr += BUFFER_SIZE;
				BUFFER_SIZE = rs;
			}

			// Read until theres nothing left
			rc = read(c_socket, ptr, rs);
			if(rc == -1) {
				++i; // Count down for 100 usec max read wait b/w valid read
				if(i < 3) { 
					if(errno == EAGAIN)
						break;
					else {
						perror("Read error");
						goto fof;
					}
				}
				usleep(50); // wait for more information, 50 usec
			}
			else
				i = 0;
			ptr += rc;
		} while(rc != 0);
		++ptr;
		ptr = '\0';

    fcntl(c_socket, F_SETFL, flags);

		// Figure out what we should look up
		{
			ptr = strtok_r(buffer, del, &tok);
			if(!ptr) // there was no message we can work with
				goto fof;
			// We just want the GET and the path
			if(strncmp(ptr, "GET", 3))
				goto fof;
			
			ptr = strtok_r(NULL, del, &tok);
			if(!ptr)
				goto fof;
			if(ptr[0] != '/')
				goto fof;

			// we want to find the length of the path
			rs = strtok_r(NULL, del, &tok) - ptr;
		}


		++ptr;
		sz = strlen(ptr);
		if(!sz)
			goto fof;
		file = getval_n_l(file_list,ptr,sz);
		if(!file)
			goto fof;
		ptr = (char * restrict) file->data;
		sz = file->size;
		for(i = 0; i < sz; ) {
		  rc = write( c_socket, &ptr[i], sz-i); 
		  write( 0, &ptr[i], sz-i); 
		  if( rc < 0 ) {
		    perror("Could not write");
				goto close;
		  }
		  i += rc;
		}
		ptr = endtrans;
		rc = write( c_socket, &ptr, 2);
		goto close;

fof:
		
		write(c_socket, fourzerofour, strlen(fourzerofour));


close:
		free(val->data);
		free(val);

		if( close(c_socket) == -1 ) {
		  perror("Could not close c_socket");
			goto retry;
		}

	}

	free(buffer);

	return NULL;
}


