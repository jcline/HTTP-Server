#ifndef CCONTROL_H
#define CCONTROL_H

#include "include.h"

extern size_t MAX_CLIENT_THREADS;

void cc_start(struct list_t* fl, int iterations, int port, char* hn, char* proxy);
void cc_stop();

#endif
