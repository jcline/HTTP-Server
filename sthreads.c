#include "include.h"

void * st_thread(void* args) {
	assert(args);

	struct st_args_t * params = (struct st_args_t *) args;

	struct list_t * request_list = params->request_list,
								* file_list = params->file_list;

	assert(params->request_list);
	assert(params->file_list);

	struct node_t* restrict val;
	ssize_t i = 0, sz = 0;
	int c_socket, rc;
	char* ptr = NULL, *endtrans = "\n\r";
	while(1) { 
retry:
		val = pop_front_n(request_list);
		printf("stuffing %d\t", params->done);

		if(params->done)
			return NULL;

		if(!val)
			continue;

		sz = val->size;
		c_socket = val->misc;
		printf("%d\t", sz);
		printf("%d\n", c_socket);

		goto writer;
		do {
			rc = read(c_socket, ptr, 250);
			ptr += rc;
			if(rc == -1) {
				perror("Read error");
				goto retry;
			}
		} while(rc != 0);
writer:

		ptr = getval(file_list,0);
		for(i = 0; i < sz; ++i) {
		  rc = write( c_socket, &ptr[i], sz-i); 
		  if( rc < 0 ) {
		    perror("Could not write");
		    break;
		  }
		  i += rc;
		}
		ptr = endtrans;
		rc = write( c_socket, &ptr, 2);


		if( close(c_socket) == -1 ) {
		  perror("Could not close c_socket");
		  exit(1);
		}

		free(val->data);
		free(val);

	}
	printf("wat\n");

	return NULL;
}
