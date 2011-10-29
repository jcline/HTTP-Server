#include "include.h"

size_t MAX_READ_THREADS = 1;
size_t MAX_CLIENT_THREADS = 100;

int main(int argc, char** argv) {
	char host[255], buf[255], proxy[255];
	DIR * dp;
	int p_port, s_port, trials, use_proxy = 0;
	size_t count = 0;
	struct list_t file_list;
	struct dirent *entries;

	switch (argc) {
		case 7:
			strcpy(proxy, argv[5]);
			p_port = atoi(argv[6]);
			use_proxy = 1;
		case 5:
			strcpy(host,argv[1]);
			s_port = atoi(argv[2]);
			trials = atoi(argv[3]);
			MAX_CLIENT_THREADS = atoi(argv[4]);
			break;
		default:
			fprintf(stderr, "Not enough arguments!\n");
			exit(1);
			break;
	}

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


	if(use_proxy)
		cc_start(&file_list, trials, s_port, host, proxy);
	else
		cc_start(&file_list, trials, s_port, host, NULL);

	cc_stop();
	destroy(&file_list);
	return 0;
}
