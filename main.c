#include "include.h"

size_t MAX_READ_THREADS = 10;

int main(int argc, char** argv) {
	// Start logging for error messages etc
	start_logging();


	stop_logging();

	char* restrict root_dir = malloc(sizeof(char)*5);
	strcpy(root_dir,".");

	rc_startup();
	printf("rt started\n");
	rc_readdir(root_dir);
	printf("dir read spawn\n");
	rc_stop();

	return 0;
}
