#ifndef NET_H
#define NET_H

#include "include.h"

size_t s_data(int fd, const char* buf, size_t size);
ssize_t sp_control(int fd[2], int out, int in, size_t size);
ssize_t sp_data(int out, int in, size_t size);
size_t r_data_tv(int fd, char** buf, size_t* bs, const char* stop, size_t ss, size_t count, struct timeval* restrict initial_resp);
size_t r_data_tv_c(int fd, char** buf, size_t* bs, struct timeval* restrict initial_resp);

#endif
