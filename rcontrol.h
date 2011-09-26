#ifndef RCONTROL_H
#define RCONTROL_H

#include "include.h"

extern size_t MAX_READ_THREADS;

struct rc_file_t{
	char* name;
	char* data;
	size_t size;
};


struct list_t * rc_startup();
void rc_readdir(char* restrict dir);
void rc_readfile(char* restrict name);
void rc_stop();

#endif
