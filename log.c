#include "include.h"

#define MAX_MESSAGES 1000
#define MAX_MESSAGE_LENGTH 250

// A structure to hold the arguments to the logging() thread.
// Future proof, just add more members to pass more arguments.
struct log_thread_arg_s{
};

// Message structure
struct message_t{
	char data[MAX_MESSAGE_LENGTH];
	size_t len;
	enum LEVELS lvl;
	struct message_t* next;
} *head;
// Lock for the head pointer
pthread_mutex_t head_lock;

static pthread_t* thread_id;

// This mutex shall be locked until it is time to end the logging thread
// That is, the condition for logging() to terminate is when it acquires
// this lock
pthread_mutex_t end_signal;

int start_logging() {
	//struct log_thread_arg_s * args;
	int i;

	thread_id = (pthread_t*) malloc(sizeof(pthread_t));

	// Initialize arguments, currently none, thus commented out
	/*
	args = (struct log_thread_arg_s*) malloc(sizeof(struct log_thread_arg_s));
	if(!args) {
		fprintf(stderr, "ERROR: Could not malloc logging thread arguments.\n");
		exit(2);
	}
	*/

	pthread_mutex_init(&end_signal, NULL);
	while(pthread_mutex_lock(&end_signal)); // Aquire lock

	//message_list_capacity = 0;
	head = NULL;
	pthread_mutex_init(&head_lock, NULL);

	for(i = 0;
		 	//i < 5 && pthread_create(thread_id, NULL, logger, (void*) args); // uncomment if adding arguments
			i < 5 && pthread_create(thread_id, NULL, logger, NULL);
		 	++i);

	if(i == 5) {
		fprintf(stderr, "ERROR: Could not create logging thread.\n");
		exit(1);
	}

	return 0;
}

int stop_logging() {

	// Unlock to indicate logging() should terminate
	pthread_mutex_unlock(&end_signal);
	// Wait for logging() to clean up and terminate
	pthread_join(*thread_id, NULL);
	// Clean up mutex
	pthread_mutex_destroy(&end_signal);
	// free thread_id
	free(thread_id);

	return 0;
}

int log_message(enum LEVELS lvl, const char* str, size_t len) {

	struct message_t msg;
	msg.lvl = lvl;
	msg.len = (len < MAX_MESSAGE_LENGTH) ? len : MAX_MESSAGE_LENGTH - 1;
	memcpy(msg.data, str, msg.len);
	msg.data[len] = '\0';

	int r;
	/*
	while(pthread_mutex_lock(head)) {
		if(head == NULL) {
		}
	}
	*/

	return 0;
}

void *logger(/*void* args*/) {
	while(pthread_mutex_trylock(&end_signal)) {
		/*
		switch(lvl) {
			case MESSAGE:
				fprintf(stdout, "INFO: %s", msg->data);
				break;
			case WARNING:
				fprintf(stdout, "WARNING: %s", msg->data);
				break;
			case ERROR:
				fprintf(stderr, "ERROR: %s", msg->data);
				break;
		}
		*/
	}
	// free(args);
	return NULL;
}
