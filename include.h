#ifndef INCLUDES_H
#define INCLUDES_H

#define _REENTRANT
#define _GNU_SOURCE //splice

// stdlib includes

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// System includes
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// Project files
// Data structures
#include "hash.h"
#include "list.h"
//   Networking
#include "net.h"
//   Threading
#include "cthreads.h"
#include "ccontrol.h"
#include "pthreads.h"
#include "pcontrol.h"
#include "sthreads.h"
#include "scontrol.h"
//   Logging
#include "log.h"

#endif

