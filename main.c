#include "include.h"

size_t MAX_READ_THREADS = 1;

int main(int argc, char** argv) {
	// Start logging for error messages etc
	start_logging();


	stop_logging();

	char* restrict root_dir = malloc(sizeof(char)*5);
	strcpy(root_dir,".");

	rc_startup();
	printf("started\n");
	rc_readdir(root_dir);
	printf("reading\n");
	sleep(5);
	rc_stop();
	printf("stopping\n");

	return 0;
}
