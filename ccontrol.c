#include "include.h"

static struct list_t * file_list;
static struct ct_args_t * args;

static int init_check = 0;

static pthread_t* cthreads;
struct ct_stats_t* statarr;

static size_t numr = 0;

void cc_start(struct list_t* fl, int iterations, int port, char* hn) {
  assert(!init_check);
  init_check = 1;
  file_list = fl;

	numr = iterations;

	struct hostent* s_info;
	s_info = gethostbyname(hn);


  cthreads = (pthread_t*) malloc(sizeof(pthread_t)*MAX_CLIENT_THREADS);
	statarr = (struct ct_stats_t *) malloc(sizeof(struct ct_stats_t)*MAX_CLIENT_THREADS);

  int i;
  for(i = 0; i < MAX_CLIENT_THREADS; ++i) {
			args = (struct ct_args_t *) malloc(sizeof(struct ct_args_t));
			assert(args);
			args->file_list = file_list;
			args->done = iterations;
			args->s_info = s_info;
			args->s_port = port;
			args->stats = &statarr[i];
			args->stats->OK = 0;
			args->stats->BAD = 0;
			args->stats->FOUND = 0;
			args->stats->IMPL = 0;

      //cthreads[i] = (pthread_t *) malloc(sizeof(pthread_t));
      //assert(cthreads[i]);
      pthread_create(&cthreads[i], NULL, ct_thread, (void*) args);
  }

}

void cc_stop() {
	assert(init_check);

	int i, j;
	for(i = 0; i < MAX_CLIENT_THREADS; ++i) {
		pthread_join(cthreads[i], NULL);
	}

	struct ct_stats_t stats;
	stats.OK = 0;
	stats.BAD = 0;
	stats.FOUND = 0;
	stats.IMPL = 0;

	double ttime = 0,
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

		free(statarr[i].rtimes);
		free(statarr[i].ftimes);
		free(statarr[i].dtimes);
	}
	free(statarr);
	
	double ttimeavg = ttime/MAX_CLIENT_THREADS;
	double rtimeavg = rtime/(MAX_CLIENT_THREADS*numr);
	double ftimeavg = ftime/(MAX_CLIENT_THREADS*numr);
	double dtimeavg = dtime/(MAX_CLIENT_THREADS*numr);

	char* tta_unit = "us";
	if(ttimeavg > 1000) {
		ttimeavg /= 1000;
		if(ttimeavg > 1000) {
			ttimeavg /= 1000;
			tta_unit = "s";
		}
		else
			tta_unit = "ms";
	}

	char* ttmin_unit = "us";
	if(ttimemin > 1000) {
		ttimemin /= 1000;
		if(ttimemin > 1000) {
			ttimemin /= 1000;
			ttmin_unit = "s";
		}
		else
			ttmin_unit = "ms";
	}

	char* ttmax_unit = "us";
	if(ttimemax > 1000) {
		ttimemax /= 1000;
		if(ttimemax > 1000) {
			ttimemax /= 1000;
			ttmax_unit = "s";
		}
		else
			ttmax_unit = "ms";
	}

	char* fta_unit = "us";
	if(ftimeavg > 1000) {
		ftimeavg /= 1000;
		if(ftimeavg > 1000) {
			ftimeavg /= 1000;
			fta_unit = "s";
		}
		else
			fta_unit = "ms";
	}

	char* ftmin_unit = "us";
	if(ftimemin > 1000) {
		ftimemin /= 1000;
		if(ftimemin > 1000) {
			ftimemin /= 1000;
			ftmin_unit = "s";
		}
		else
			ftmin_unit = "ms";
	}

	char* ftmax_unit = "us";
	if(ftimemax > 1000) {
		ftimemax /= 1000;
		if(ftimemax > 1000) {
			ftimemax /= 1000;
			ftmax_unit = "s";
		}
		else
			ftmax_unit = "ms";
	}

	char* rta_unit = "us";
	if(rtimeavg > 1000) {
		rtimeavg /= 1000;
		if(rtimeavg > 1000) {
			rtimeavg /= 1000;
			rta_unit = "s";
		}
		else
			rta_unit = "ms";
	}

	char* rtmin_unit = "us";
	if(rtimemin > 1000) {
		rtimemin /= 1000;
		if(rtimemin > 1000) {
			rtimemin /= 1000;
			rtmin_unit = "s";
		}
		else
			rtmin_unit = "ms";
	}

	char* rtmax_unit = "us";
	if(rtimemax > 1000) {
		rtimemax /= 1000;
		if(rtimemax > 1000) {
			rtimemax /= 1000;
			rtmax_unit = "s";
		}
		else
			rtmax_unit = "ms";
	}

	char* dta_unit = "us";
	if(dtimeavg > 1000) {
		dtimeavg /= 1000;
		if(dtimeavg > 1000) {
			dtimeavg /= 1000;
			dta_unit = "s";
		}
		else
			dta_unit = "ms";
	}

	char* dtmin_unit = "us";
	if(dtimemin > 1000) {
		dtimemin /= 1000;
		if(dtimemin > 1000) {
			dtimemin /= 1000;
			dtmin_unit = "s";
		}
		else
			dtmin_unit = "ms";
	}

	char* dtmax_unit = "us";
	if(dtimemax > 1000) {
		dtimemax /= 1000;
		if(dtimemax > 1000) {
			dtimemax /= 1000;
			dtmax_unit = "s";
		}
		else
			dtmax_unit = "ms";
	}

	printf("Stats: 200: %ld\t400: %ld\t404: %ld\t501: %ld\n",
			stats.OK, stats.BAD,
			stats.FOUND, stats.IMPL );
	printf("Average time to complete all trials:\t\t\t%f %s\n",
			ttimeavg, tta_unit);
	printf("Minimum time to complete a trial:\t\t\t%f %s\n",
			ttimemin, ttmin_unit);
	printf("Maximum time to complete a trial:\t\t\t%f %s\n",
			ttimemax, ttmax_unit);


	printf("Average time for the server to respond:\t\t\t%f %s\n",
			rtimeavg, rta_unit);
	printf("Minimum time for the server to respond:\t\t\t%f %s\n",
			rtimemin, rtmin_unit);
	printf("Maximum time for the server to respond:\t\t\t%f %s\n",
			rtimemax, rtmax_unit);

	printf("Average time to complete one trial:\t\t\t%f %s\n",
			ftimeavg, fta_unit);
	printf("Minimum time to complete one trial:\t\t\t%f %s\n",
			ftimemin, ftmin_unit);
	printf("Maximum time to complete one trial:\t\t\t%f %s\n",
			ftimemax, ftmax_unit);

	printf("Average time to receive all data from the server:\t%f %s\n",
			dtimeavg, dta_unit);
	printf("Minimum time to receive all data from the server:\t%f %s\n",
			dtimemin, dtmin_unit);
	printf("Maximum time to receive all data from the server:\t%f %s\n",
			dtimemax, dtmax_unit);

	free(cthreads);
}
