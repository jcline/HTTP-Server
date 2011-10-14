#include "net.h"

size_t s_data(int fd, const char* buf, size_t size) {
	size_t chunk = size, r = 0, rc = 0;
	do {
		rc = write(fd, buf, chunk);
		printf("%d-", rc);
		if(rc < 0) {
			perror("write failed");
		}
		r += rc;
		chunk -= rc;
	} while(r < size);
	return r;
}

size_t r_data_tv(int fd, char** buf, size_t* bs, const char* stop, size_t ss, struct timeval* restrict initial_resp) {
	size_t chunk = *bs - 1, r = 0, rc = 0, ndone = 1, fi = 1, i, j, k;
	char* ptr = *buf;
	do {
		rc = read(fd, ptr, chunk);
		if(rc < 0) {
			perror("read failed");
			ndone = 0;
		}
		else if(!rc)

			ndone = 0;
		if(fi) {
			if(initial_resp)
				gettimeofday(initial_resp, NULL);
			fi = 0;
		}

		printf(" %d ", rc);
		fflush(stdout);

		r += rc;
		if(rc) {
			for(i = r-rc; i < r; ++i) {
				if(buf[i] == stop[0]) {
					ndone = 0;
					k = i;
					++i;
					for(j = 1; j < ss && i < r; ++j, ++i) {
						if(buf[i] != stop[j]) {
							ndone = 1;
						}
					}
					if(ndone || ( i == r && j != ss))
						i = k;
				}
			}
		}
#ifndef NDEBUG
		//printf("%d\t %d, %d\n", fd, r, ndone);
#endif

		chunk -= rc;
		if(ndone && !chunk) {
			size_t ns = *bs;
			chunk = ns/2;
			*bs += chunk;
			errno = 0;
			ptr = realloc(*buf, *bs);
			if(!ptr) {
				perror("realloc failed");
			}
			else {
				*buf = ptr;
				memset(*buf+ns, 0, chunk);
			}
			--chunk;
		}
		ptr = *buf + r - 1;
	} while(ndone);
	return r;
}
