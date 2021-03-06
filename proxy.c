#include "include.h"

size_t MAX_PROXY_THREADS = 10;

int main(int argc, char** argv) {
	int port, shared_memory;

	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);

	struct sigaction sac;
	sac.sa_handler = pc_kill;
	sigaction(SIGINT, &sac, NULL);

	if( argc < 4 ) {
		fprintf(stderr, "Not enough arguments!\n");
		exit(1);
	}

	port = atoi(argv[1]);
	MAX_PROXY_THREADS = atoi(argv[2]);
	shared_memory = ( !strcmp(argv[3], "shared") ) ? 1 : 0;
	if(shared_memory) {
		printf("Running in shared memory mode\n");
	} 
	else {
		printf("Running in socket mode\n");
	}

	pc_start(port, shared_memory);
	pc_stop();

	return 0;
}

