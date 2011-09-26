#include "include.h"

static struct list_t * file_list;
static struct list_t request_list;
static struct st_args_t * args;

static int init_check = 0;
int s_addr_sz = sizeof(struct sockaddr_in); // !!

static pthread_t** sthreads;
static pthread_t manager;

void * sc_manager(void* args) {

  s_socket = socket(AF_INET,SOCK_STREAM,0);
  if(s_socket == -1) {
    fprintf(stderr, "ERROR: Could not make s_socket.\n");
    exit(1);
  }

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

  int c_socket, rc;
  char buffer[250];
  char * nul_loc = &buffer[249];

  printf("listening\n");
  while(1) {
    c_socket = accept( s_socket, NULL, (socklen_t *) s_addr_sz);
    printf("new connection\n");

    rc = read( c_socket, buffer, 249);
    *nul_loc = '\0';

    if(rc > 0)
      push_back(&request_list, buffer, rc, NULL, c_socket);
    else
      perror("read error");

  }


  return NULL;
}

void sc_start(struct list_t* fl, int port) {
  assert(!init_check);
  init_check = 1;
  init(&request_list);
  file_list = fl;

  s_port = port;

  args = (struct st_args_t *) malloc(sizeof(struct st_args_t));
  args->file_list = file_list;
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
  
}

void sc_stop() {
	assert(init_check);
	//join
//	args->done = 1;
	int i;
	for(i = 0; i < MAX_SERVE_THREADS; ++i) {
		pthread_join(*(sthreads[i]), NULL);
	}

	free(args);

	destroy(&request_list);

	for(i = 0; i < MAX_SERVE_THREADS; ++i) {
		free(sthreads[i]);
	}

	free(sthreads);
}
