#ifndef LOG_H
#define LOG_H

#include "include.h"

enum LEVELS{
	MESSAGE,
	WARNING,
	ERROR
};

pthread_mutex_t exit_signal;

int start_logging();
int stop_logging();
int log_message(enum LEVELS lvl, const char* str, size_t len);
void *logger(/*void* args*/);

#endif
