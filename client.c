#include "include.h"

size_t MAX_READ_THREADS = 1;
size_t MAX_CLIENT_THREADS = 100;

int main(int argc, char** argv) {
	char host[255], buf[255];
	DIR * dp;
	int port, trials;
	size_t count = 0;
	struct list_t file_list;
	struct dirent *entries;

	if( argc < 4 ) {
		fprintf(stderr, "Not enough arguments!\n");
		exit(1);
	}

	strcpy(host,argv[1]);
	port = atoi(argv[2]);
	trials = atoi(argv[3]);
	MAX_CLIENT_THREADS = atoi(argv[4]);

	init(&file_list);

	buf[0]='/';
	dp = opendir(".");
	if(dp) {
		while( (entries = readdir(dp)) ) {
			if(entries->d_name[0] == '.')
				continue;
			strcpy(&(buf[1]), entries->d_name);
			push_back(&file_list, buf, strlen(entries->d_name)+1,
					NULL, 0);
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


	cc_start(&file_list, trials, port, host);

	cc_stop();
	destroy(&file_list);
	return 0;
}
