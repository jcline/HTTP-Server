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
#include <unistd.h>

// System includes
#include <dirent.h>
#include <pthread.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/types.h>

// Project files
// Data structures
#include "list.h"
//   Threading
#include "sthreads.h"
#include "scontrol.h"
#include "rthreads.h"
#include "rcontrol.h"
//   Logging
#include "log.h"

#endif

