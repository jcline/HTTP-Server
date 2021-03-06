#include "include.h"

static struct list_t request_list;
static struct st_args_t ** args;
static int s_socket, s_port;
static struct sockaddr_in s_addr;

static int init_check = 0, use_shared = 0, stop = 0;
int s_addr_sz = sizeof(struct sockaddr_in); // !!

static pthread_t** sthreads;
static pthread_t manager, shm_manager;

void * sc_manager(void* args) {

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
#ifndef NDEBUG 
    printf("new connection\t%d\n", c_socket);
#endif

		push_back(&request_list, buffer, 1, NULL, c_socket);

  }

  return NULL;
}

void sc_start(int port, int us) {
  assert(!init_check);
  init_check = 1;
  init(&request_list);

  s_port = port;
	use_shared = us;

  args = (struct st_args_t **) malloc(sizeof(struct st_args_t*)*MAX_SERVE_THREADS);
	struct shm_thread_t ** t_t = NULL;
	if(use_shared)
		t_t = (struct shm_thread_t**) malloc(sizeof(struct shm_thread_t*)*MAX_SERVE_THREADS);

  sthreads = (pthread_t**) malloc(sizeof(pthread_t)*MAX_SERVE_THREADS);

  int i;
  for(i = 0; i < MAX_SERVE_THREADS; ++i) {
		args[i] = (struct st_args_t*) malloc(sizeof(struct st_args_t));
		if(use_shared)
			t_t[i] = (struct shm_thread_t*) malloc(sizeof(struct shm_thread_t));
	
		args[i]->request_list = &request_list;
		args[i]->done = 0;
		args[i]->use_shared = us;
		args[i]->id = i;
		args[i]->share = t_t;
		if(use_shared) {
			if( shared_manage( &t_t[i], &(args[i]->shmid), i+0xab, sizeof(struct shm_thread_t)) )
				exit(1);
			t_t[i]->web = 1;
			if(!t_t[i]->init) {
				{
					pthread_mutexattr_t attr;
					pthread_mutexattr_init(&attr);
					if(pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) perror("shm_mutex");
					if(pthread_mutex_init(&(t_t[i]->lock), &attr)) perror("shm_mutex_init");
					pthread_mutexattr_destroy(&attr);
				}

				{
					pthread_condattr_t attr;
					pthread_condattr_init(&attr);
					if(pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) perror("shm_cond");
					if(pthread_cond_init(&(t_t[i]->sig), &attr)) perror("shm_cond_init");
					pthread_condattr_destroy(&attr);
				}

				t_t[i]->init = 1;
			}
		}
		else
			t_t = NULL;

		sthreads[i] = (pthread_t *) malloc(sizeof(pthread_t));
		assert(sthreads[i]);
		pthread_create(sthreads[i], NULL, st_thread, (void*) args[i]);
  }

  pthread_create(&manager, NULL, sc_manager, NULL);

}

void sc_stop() {
	assert(init_check);
	//join

	while(!stop) { sleep(1); }

	printf("Stopping\n");

	int i;
	for(i = 0; i < MAX_SERVE_THREADS; ++i) {
		pthread_kill(*(sthreads[i]), SIGUSR1);
	}

	usleep(50000);

	printf("Threads signaled\n");
	request_list.stop = 1;
	pthread_cond_broadcast(&request_list.work);

	for(i = 0; i < MAX_SERVE_THREADS; ++i) {
		pthread_join(*(sthreads[i]), NULL);
	}

	destroy(&request_list);

	for(i = 0; i < MAX_SERVE_THREADS; ++i) {
		free(sthreads[i]);
		free(args[i]);
	}

	free(args);
	free(sthreads);
}

void sc_kill() {
	stop = 1;
}

void sc_signal(int id, int signal) {
	printf("signaling\n");
	pthread_kill(*(sthreads[id]), SIGUSR2);
}
