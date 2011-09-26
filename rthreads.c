#include "rthreads.h"

void *rt_thread(void* args) {
	assert(args);

	struct rt_args_t * params = (struct rt_args_t *) args;

	struct list_t * filename_list = params->filename_list,
								* file_list = params->file_list;

	assert(params->filename_list);
	assert(params->file_list);

	void* restrict val;
	FILE* restrict fp;
	size_t sz = 0;
	char* restrict fd;
	while(1) { 
		val = pop_front(filename_list);
		//printf("stuffing %d\t", params->done);

		if(params->done)
			return NULL;

		if(!val)
			continue;

		// We don't want . files.
		if(((char*)val)[0] == '.')
			continue;

		//printf("%s\n", val);

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
