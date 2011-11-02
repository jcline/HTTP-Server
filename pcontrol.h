#ifndef PCONTROL_H
#define PCONTROL_H

#include "include.h"

extern size_t MAX_PROXY_THREADS;

void pc_start(int port, int us);
void pc_stop();
void pc_kill();

#endif
