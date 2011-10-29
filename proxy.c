#include "include.h"

size_t MAX_PROXY_THREADS = 10;

int main(int argc, char** argv) {
	char host[255], buf[255];
	int port;
	size_t shared_memory;

	if( argc < 4 ) {
		fprintf(stderr, "Not enough arguments!\n");
		exit(1);
	}

	strcpy(host,argv[1]);
	port = atoi(argv[2]);
	MAX_PROXY_THREADS = atoi(argv[3]);
	shared_memory = ( !strcmp(argv[4], "shared") ) ? 1 : 0;
	if(shared_memory) {
		printf("Running in shared memory mode\n");
	} 
	else {
		printf("Running in socket mode\n");
	}

	pc_start(port);
	pc_stop();

	return 0;
}
