#ifndef INCLUDES_H
#define INCLUDES_H

#define _REENTRANT

// stdlib includes

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// System includes
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// Project files
// Data structures
#include "list.h"
//   Threading
#include "cthreads.h"
#include "ccontrol.h"
#include "sthreads.h"
#include "scontrol.h"
#include "rthreads.h"
#include "rcontrol.h"
//   Logging
#include "log.h"

#endif

