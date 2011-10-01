#include "include.h"

size_t MAX_READ_THREADS = 1;
size_t MAX_SERVE_THREADS = 0;

int main(int argc, char** argv) {
	if( argc < 3 ) {
		fprintf(stderr, "Not enough arguments!\n");
		exit(1);
	}

	int port;
	port = atoi(argv[1]);
	MAX_SERVE_THREADS = atoi(argv[2]);

	char* restrict root_dir = malloc(sizeof(char)*5);
	strcpy(root_dir,".");

	struct list_t * file_list = rc_startup();
	printf("started\n");
	rc_readdir(root_dir);
	printf("reading\n");

	sc_start(file_list, port);

	sc_stop();
	rc_stop();
	return 0;
}
