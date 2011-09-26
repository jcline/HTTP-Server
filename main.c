#include "include.h"

size_t MAX_READ_THREADS = 1;
size_t MAX_SERVE_THREADS = 1;

int main(int argc, char** argv) {
	// Start logging for error messages etc
	start_logging();


	stop_logging();

	char* restrict root_dir = malloc(sizeof(char)*5);
	strcpy(root_dir,".");

	struct list_t * file_list = rc_startup();
	printf("started\n");
	rc_readdir(root_dir);
	printf("reading\n");

	sc_start(file_list, 8080);

	sc_stop();
	rc_stop();
	return 0;
}
