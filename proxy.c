#include "include.h"

size_t MAX_PROXY_THREADS = 10;

int main(int argc, char** argv) {
	int port;

	if( argc < 3 ) {
		fprintf(stderr, "Not enough arguments!\n");
		exit(1);
	}

	port = atoi(argv[1]);
	MAX_PROXY_THREADS = atoi(argv[2]);

	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);

	pc_start(port);
	pc_stop();

	return 0;
}
