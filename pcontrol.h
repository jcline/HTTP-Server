#ifndef PCONTROL_H
#define PCONTROL_H

#include "include.h"

extern size_t MAX_PROXY_THREADS;

void * pc_manager(void* args);
void pc_start(int port, char* host);
void pc_stop();

#endif
