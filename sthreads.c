#include "include.h"

void * st_thread(void* args) {
	assert(args);

	struct st_args_t * params = (struct st_args_t *) args;

	struct list_t * request_list = params->request_list,
								* file_list = params->file_list;

	assert(params->request_list);
	assert(params->file_list);

	void* restrict val;
	ssize_t i = 0, sz = 0;
	int c_socket, rc;
	char* ptr = NULL;
	struct node_t* node;
	while(1) { 
		val = pop_front(request_list);
		printf("stuffing %d\t", params->done);

		if(params->done)
			return NULL;

		printf("%d\n", sz);
		if(!val)
			continue;

		node = (struct node_t *) val;


		sz = node->size;
		c_socket = node->misc;

		printf("%d\n", c_socket);
		ptr = getval(file_list,5);
		for(i = 0; i < sz; ++i) {
		  rc = write( c_socket, &ptr[i], sz); 
		  if( rc < 0 ) {
		    perror("Could not write");
		    break;
		  }
		  i += rc;
		}


		if( close(c_socket) == -1 ) {
		  perror("Could not close c_socket");
		  exit(1);
		}

		free(node->data);
		free(val);

	}
	printf("wat\n");

	return NULL;
}
