#include "include.h"

static struct list_t * file_list;
static struct ct_args_t * args;

static int init_check = 0;

static pthread_t** cthreads;
struct ct_stats_t* statarr;

static size_t numr;

void cc_start(struct list_t* fl, int iterations, int port, char* hn) {
  assert(!init_check);
  init_check = 1;
  file_list = fl;

	numr = iterations;

	struct hostent* s_info;
	s_info = gethostbyname(hn);


  cthreads = (pthread_t**) malloc(sizeof(pthread_t)*MAX_CLIENT_THREADS);
	statarr = (struct ct_stats_t *) malloc(sizeof(struct ct_stats_t)*MAX_CLIENT_THREADS);

  int i;
  for(i = 0; i < MAX_CLIENT_THREADS; ++i) {
			args = (struct ct_args_t *) malloc(sizeof(struct ct_args_t));
			args->file_list = file_list;
			args->done = iterations;
			args->s_info = s_info;
			args->s_port = port;
			args->stats = &statarr[i];
			args->stats->OK = 0;
			args->stats->BAD = 0;
			args->stats->FOUND = 0;
			args->stats->IMPL = 0;

      cthreads[i] = (pthread_t *) malloc(sizeof(pthread_t));
      assert(cthreads[i]);
      pthread_create(cthreads[i], NULL, ct_thread, (void*) args);
  }

}

void cc_stop() {
	assert(init_check);

	int i, j;
	for(i = 0; i < MAX_CLIENT_THREADS; ++i) {
		pthread_join(*(cthreads[i]), NULL);
	}

	for(i = 0; i < MAX_CLIENT_THREADS; ++i) {
		free(cthreads[i]);
	}

	struct ct_stats_t stats;
	stats.OK = 0;
	stats.BAD = 0;
	stats.FOUND = 0;
	stats.IMPL = 0;

	unsigned long long int ttime = 0,
			 ttimemax = statarr[0].ttime,
			 ttimemin = statarr[0].ttime,
			 rtime = 0,
			 rtimemax = 0,
			 rtimemin = statarr[0].rtimes[0],
			 ftime = 0,
			 ftimemax = 0,
			 ftimemin = statarr[0].ftimes[0],
			 dtime = 0,
			 dtimemax = 0,
			 dtimemin = statarr[0].dtimes[0];

	for(i = 0; i < MAX_CLIENT_THREADS; ++i) {
		stats.OK += statarr[i].OK;
		stats.BAD += statarr[i].BAD;
		stats.FOUND += statarr[i].FOUND;
		stats.IMPL += statarr[i].IMPL;

		ttime += statarr[i].ttime;
		ttimemax = (statarr[i].ttime > ttimemax) ? statarr[i].ttime : ttimemax;
		ttimemin = (statarr[i].ttime < ttimemin) ? statarr[i].ttime : ttimemin;

		for(j = 0; j < numr; ++j) {
			long long int rt = statarr[i].rtimes[j],
					 ft = statarr[i].ftimes[j],
					 dt = statarr[i].dtimes[j];

			rtime += rt;
			ftime += ft;
			dtime += dt;

			rtimemin = (rt < rtimemin) ? rt : rtimemin;
			ftimemin = (ft < ftimemin) ? ft : ftimemin;
			dtimemin = (dt < dtimemin) ? dt : dtimemin;

			rtimemax = (rt > rtimemax) ? rt : rtimemax;
			ftimemax = (ft > ftimemax) ? ft : ftimemax;
			dtimemax = (dt > dtimemax) ? dt : dtimemax;
		}

	}
	double ttimeavg = ttime/MAX_CLIENT_THREADS;
	double rtimeavg = rtime/(MAX_CLIENT_THREADS*numr);
	double ftimeavg = ftime/(MAX_CLIENT_THREADS*numr);
	double dtimeavg = dtime/(MAX_CLIENT_THREADS*numr);

	printf("Stats: 200: %ld\t400: %ld\t404: %ld\t501: %ld\n",
			stats.OK, stats.BAD,
			stats.FOUND, stats.IMPL );
	printf("Average time to complete all trials: %fs\n",
			(double)(ttimeavg/(1000*1000)));
	printf("Minimum time to complete a trial: %fs\n",
			(double)(ttimemin/(1000*1000)));
	printf("Maximum time to complete a trial: %fs\n",
			(double)(ttimemax/(1000*1000)));


	printf("Average time for the server to respond: %fus\n",
			rtimeavg);
	printf("Minimum time for the server to respond: %Ldus\n",
			rtimemin);
	printf("Maximum time for the server to respond: %Ldus\n",
			rtimemax);

	printf("Average time to complete one trial: %fms\n",
			(double)(ftimeavg/1000));
	printf("Minimum time to complete one trial: %fms\n",
			(double)(ftimemin/1000));
	printf("Maximum time to complete one trial: %fms\n",
			(double)(ftimemax/1000));

	printf("Average time to receive all data from the server: %fus\n",
			dtimeavg);
	printf("Minimum time to receive all data from the server: %Ldus\n",
			dtimemin);
	printf("Maximum time to receive all data from the server: %Ldus\n",
			dtimemax);

	free(cthreads);
}
