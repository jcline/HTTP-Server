#include "rthreads.h"

void *rt_thread(void* args) {

	struct rt_args_t * params = (struct rt_args_t *) args;

	struct list_t * filename_list = params->filename_list,
								* file_list = params->file_list;

	void* restrict val;
	FILE* restrict fp;
	size_t sz = 0;
	char* restrict fd;
	while( (val = pop_front(filename_list)) ) {
		fp = fopen((const char* restrict) val, "r");
		if(fp) {
			// Get size of file
			fseek(fp, 0L, SEEK_END);
			sz = ftell(fp);	
			fseek(fp, 0L, SEEK_SET);

			// Allocate memory
			fd = (char* restrict) malloc(sizeof(char) * sz);

			fread((void*) fd, sz, sizeof(char), fp);

			push_back(file_list, fd, sz);

			free(val);
		}
	}

	return NULL;
}
