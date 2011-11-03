#ifndef SCONTROL_H
#define SCONTROL_H

#include "include.h"

extern size_t MAX_SERVE_THREADS;

void sc_start(int port, int us);
void sc_stop();
void sc_kill();

#endif
