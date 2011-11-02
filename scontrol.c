#include "include.h"

static struct list_t request_list;
static struct st_args_t * args;
static int s_socket, s_port;
static struct sockaddr_in s_addr;

static int init_check = 0, use_shared = 0;
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

void * sc_shm_manager(void* args) {
	int shmid = 0;

	do {
		usleep(100000); // sleep for 100ms
#ifndef NDEBUG
	printf("Attempting to aquire control shm\n");
	fflush(stdout);
#endif

		shmid = shmget(0xaa, sizeof(struct shm_control_t), 0);
		if(shmid == -1) {
			if(errno == ENOENT)
				continue;
			perror("shmget");
		}
		else
			break;
	} while(1);

#ifndef NDEBUG
	printf("Aquired control shm\n");
	fflush(stdout);
#endif

	struct shm_control_t * shm_c = (struct shm_control_t *) 
		shared_mmap(shmid, sizeof(struct shm_control_t));
	shared_end(shm_c);

	return NULL;
}

void sc_start(int port, int us) {
  assert(!init_check);
  init_check = 1;
  init(&request_list);

  s_port = port;
	use_shared = us;

  args = (struct st_args_t *) malloc(sizeof(struct st_args_t));
  args->request_list = &request_list;
  args->done = 0;

  sthreads = (pthread_t**) malloc(sizeof(pthread_t)*MAX_SERVE_THREADS);

  int i;
  for(i = 0; i < MAX_SERVE_THREADS; ++i) {
      sthreads[i] = (pthread_t *) malloc(sizeof(pthread_t));
      assert(sthreads[i]);
      pthread_create(sthreads[i], NULL, st_thread, (void*) args);
  }

  pthread_create(&manager, NULL, sc_manager, NULL);

	if(use_shared) {
		pthread_create(&shm_manager, NULL, sc_shm_manager, NULL);
	}
  
}

void sc_stop() {
	assert(init_check);
	//join
//	args->done = 1;
	int i;
	for(i = 0; i < MAX_SERVE_THREADS; ++i) {
		pthread_join(*(sthreads[i]), NULL);
	}
	printf("You should never see this line.\n");

	free(args);

	destroy(&request_list);

	for(i = 0; i < MAX_SERVE_THREADS; ++i) {
		free(sthreads[i]);
	}

	free(sthreads);
}
