#ifndef SCONTROL_H
#define SCONTROL_H

#include "include.h"

extern size_t MAX_SERVE_THREADS;

extern int s_addr_sz;
int s_socket, s_port;
struct sockaddr_in s_addr;

void sc_start();
void sc_stop();

#endif
