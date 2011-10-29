#include "include.h"

size_t MAX_READ_THREADS = 1;
size_t MAX_SERVE_THREADS = 0;

int main(int argc, char** argv) {
	int port;
	size_t shared_memory;

	if( argc < 4 ) {
		fprintf(stderr, "Not enough arguments!\n");
		exit(1);
	}

	port = atoi(argv[1]);
	MAX_SERVE_THREADS = atoi(argv[2]);
	shared_memory = ( !strcmp(argv[3], "shared") ) ? 1 : 0;
	if(shared_memory) {
		printf("Running in shared memory mode\n");
	} 
	else {
		printf("Running in socket mode\n");
	}

	char* restrict root_dir = malloc(sizeof(char)*5);
	strcpy(root_dir,".");

	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);

	sc_start(port);

	sc_stop();
	return 0;
}
