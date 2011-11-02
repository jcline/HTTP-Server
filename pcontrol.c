#include "include.h"

static struct list_t request_list;
static struct pt_args_t * args;
static int s_socket, s_port;
static struct sockaddr_in s_addr;

static int init_check = 0;
int s_addr_sz = sizeof(struct sockaddr_in); // !!
int use_shared = 0;

static pthread_t** pthreads;
static pthread_t manager;

void ** thread_shm;

void * pc_manager(void* args) {

  s_socket = socket(AF_INET,SOCK_STREAM,0);
  if(s_socket == -1) {
    fprintf(stderr, "ERROR: Could not make s_socket.\n");
    exit(1);
  }

	int TRUE = 1;
	setsockopt(s_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &TRUE, sizeof(TRUE));

  s_addr.sin_addr.s_addr=INADDR_ANY;
  s_addr.sin_port=htons(s_port);
  s_addr.sin_family=AF_INET;

  if(
    bind( s_socket, (struct sockaddr*) &s_addr, sizeof(s_addr))
    == -1) {
    fprintf(stderr, "ERROR: Could not bind s_socket.\n");
    exit(1);
  }

  if( listen(s_socket, 100) == -1 ) {
    fprintf(stderr, "ERROR: Could not listen s_socket.\n");
    exit(1);
  }

  int c_socket;
  char buffer[1];

  printf("listening\n");
  while(1) {
    c_socket = accept( s_socket, NULL, (socklen_t *) &s_addr_sz);
		if(c_socket == -1) {
			perror("accept error");
			continue;
		}
#ifndef NDEBUG // We want the delegation to be as fast as possible
    printf("new connection\t%d\n", c_socket);
#endif

		push_back(&request_list, buffer, 1, NULL, c_socket);
  }


  return NULL;
}

void pc_start(int port, int us) {
  assert(!init_check);
  init_check = 1;
  init(&request_list);

  s_port = port;

  args = (struct pt_args_t *) malloc(sizeof(struct pt_args_t));
  args->request_list = &request_list;
  args->done = 0;
	args->use_shared = us;
	use_shared = us;

  pthreads = (pthread_t**) malloc(sizeof(pthread_t)*MAX_PROXY_THREADS);

	thread_shm = (void**) malloc(sizeof(void*)*MAX_PROXY_THREADS);

  int i;
  for(i = 0; i < MAX_PROXY_THREADS; ++i) {
      pthreads[i] = (pthread_t *) malloc(sizeof(pthread_t));
      assert(pthreads[i]);
      pthread_create(pthreads[i], NULL, pt_thread, (void*) args);
  }

  pthread_create(&manager, NULL, pc_manager, NULL);
  
}

void pc_stop() {
	assert(init_check);
	//join
//	args->done = 1;
	int i;
	for(i = 0; i < MAX_PROXY_THREADS; ++i) {
		pthread_join(*(pthreads[i]), NULL);
	}
	printf("You should never see this line.\n");

	free(args);

	destroy(&request_list);

	for(i = 0; i < MAX_PROXY_THREADS; ++i) {
		free(pthreads[i]);
	}

	free(pthreads);
}

void pc_kill() {
	if(!init_check)
		return;

	for(int i = 0; i < MAX_PROXY_THREADS; ++i) {
		if(use_shared)
			shared_end(thread_shm[i]);
		pthread_kill(*(pthreads[i]), SIGKILL);
	}
}
