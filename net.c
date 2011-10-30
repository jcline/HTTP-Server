#include "net.h"

size_t s_data(int fd, const char* buf, size_t size) {
	size_t chunk = size, r = 0, rc = 0;
	do {
		rc = write(fd, buf, chunk);
		if(rc < 0) {
			perror("write failed");
		}
		r += rc;
		chunk -= rc;
	} while(r < size);
	return r;
}

ssize_t sp_control(int fd[2], int out, int in, size_t size) {
	ssize_t r = 0, rt = 0;
	size_t s = (!size) ? 16384 : size;

	do {
		r = sp_data(fd[1], in, s);
#ifndef NDEBUG
		printf("spc: %d ", r);
		fflush(stdout);
#endif
		if(r == -1 || r == 0)
			break;
		r = sp_data(out, fd[0], r);
		if(r == -1 || r == 0)
			break;
		rt += r;
		if(size)
			s -= r;
	} while(s);

	return rt;
}

ssize_t sp_data(int out, int in, size_t size) {
	ssize_t rc = 0, r = 0;
	size_t s_s = (size > 16384) ? 16384 : size;
//ssize_t splice(int in_in, loff_t *off_in, int in_out,
//loff_t *off_out, size_t len, unsigned int flags);
	do {
		rc = splice(in, NULL, out, NULL, s_s, SPLICE_F_MOVE);
		if(rc == -1) {
			fprintf(stderr, "sp_data: %d,%d,%d: %s\n", out, in, errno, strerror(errno));
			return -1;
		}
#ifndef NDEBUG
		printf("spd: %d/%d ", rc, size);
		fflush(stdout);
#endif
		s_s -= rc;
		size -= rc;
		if(!s_s && size) {
			s_s = (size > 16384) ? 16384 : size;
		}
		r += rc;
		return rc;
	} while(rc != 0 && size);

	return r;
}

size_t r_data_tv(int fd, char** buf, size_t* bs, const char* stop, size_t ss, size_t count, struct timeval* restrict initial_resp) {
	size_t chunk = *bs - 1, r = 0, rc = 0, ndone = count, fi = 1, i, j, k = 0, ind=0;
	char* ptr = *buf;
	do {
		ndone = count;
		//printf("t"); //fflush(stdout);
		rc = read(fd, ptr, chunk);
		if(rc < 0) {
			perror("read failed");
			ndone = 0;
		}
		else if(!rc)
			ndone = 0;
		//printf("%d\n", rc);
		//fflush(stdout);

		if(fi) {
			if(initial_resp)
				gettimeofday(initial_resp, NULL);
			fi = 0;
		}

		r += rc;
		if(rc) {
			for(j = k; j < r; ++j) {
				k = j;
				ind = 1;
				for(i = 0; i < ss; ++i) {
					//printf("%hhX:%hhX\t", (*buf)[k], stop[i]);
					if((*buf)[k] != stop[i]) {
						ind = 0;
						break;
					}
					else
						++k;
				}
				if(ind) {
					--ndone;
					break;
				}
			}
		}

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

size_t r_data_tv_c(int fd, char** buf, size_t* bs, struct timeval* restrict initial_resp) {
	size_t chunk = *bs - 1, r = 0, fi = 1;
	ssize_t rc = 0;
	char* ptr = *buf;
	do {
#ifndef NDEBUG
		//printf("t");
		//fflush(stdout);
#endif
		rc = read(fd, ptr, chunk);
		if(rc <= 0) {
			if(fi) {
				if(initial_resp)
					gettimeofday(initial_resp, NULL);
				fi = 0;
			}
			break;
		}
#ifndef NDEBUG
		printf("%d\n", rc);
		//fflush(stdout);
#endif

		if(fi) {
			if(initial_resp)
				gettimeofday(initial_resp, NULL);
			fi = 0;
		}

		r += rc;

		chunk -= rc;
		if(!chunk) {
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
	} while(1);
	return r;
}
