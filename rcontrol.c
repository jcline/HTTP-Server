#include "rcontrol.h"

static struct list_t filename_list;
static struct list_t file_list;
static struct rt_args_t * args;

int init_check = 0;

pthread_t** rthreads;


void rc_startup() {
	assert(!init_check);
	init_check = 1;
	init(&filename_list);
	init(&file_list);

	args = (struct rt_args_t *) malloc(sizeof(struct rt_args_t));
	args->filename_list = &filename_list;
	args->file_list = &file_list;
	args->done = 0;

	{
	rthreads = (pthread_t**) malloc(sizeof(pthread_t)*MAX_READ_THREADS);

	int i;
	for(i = 0; i < MAX_READ_THREADS; ++i) {
		rthreads[i] = (pthread_t *) malloc(sizeof(pthread_t));
		assert(rthreads[i]);
		pthread_create(rthreads[i], NULL, rt_thread, (void*) args);
	}
	}
}

void rc_stop() {
	assert(init_check);
	//join
	args->done = 1;
	int i;
	for(i = 0; i < MAX_READ_THREADS; ++i) {
		pthread_join(*(rthreads[i]), NULL);
	}

	free(args);

	destroy(&filename_list);
	destroy(&file_list);

	for(i = 0; i < MAX_READ_THREADS; ++i) {
		free(rthreads[i]);
	}

	free(rthreads);
}

void rc_readdir(char* restrict dir) {
	assert(init_check);
	DIR * dp;
	struct dirent *entries;
	size_t count = 0;

	dp = opendir(dir);
	if(dp) {
		while( (entries = readdir(dp)) ) {
			rc_readfile(entries->d_name);
			++count;
		}
		(void) closedir(dp);
	}
	else {
		perror("Could not open root directory!");
	}

	if(!count) {
		fprintf(stderr, "No files in root directory, nothing to do.\n");
		exit(1);
	}
}

void rc_readfile(char* restrict name) {
	push_back(&filename_list, name, strlen(name)+1);
}


