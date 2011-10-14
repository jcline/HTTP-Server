#ifndef NET_H
#define NET_H

#include "include.h"

size_t s_data(int fd, const char* buf, size_t size);
size_t r_data_tv(int fd, char** buf, size_t* bs, const char* stop, size_t ss, struct timeval* restrict initial_resp);

#endif
