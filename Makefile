CC			= gcc
CFLAGS		= -Wall -std=gnu99 -fstrict-aliasing ${DEBUGFLAG} 
#CFLAGS		= -Wall -std=gnu99 -fstrict-aliasing ${OPTFLAG} -DNDEBUG
OPTFLAG		= -O2
DEBUGFLAG	= -g3 -ggdb 
LINKER		= gcc
LFLAGS		= -lpthread

SRC = \
			list.c \
			sthreads.c \
			rthreads.c \
			scontrol.c \
			rcontrol.c \
			log.c \
			main.c


OBJS = $(SRC:.c=.o)

webserver: $(OBJS)
	$(LINKER) $(LFLAGS) $(OBJS) -o $@

clean:
	rm $(OBJS) webserver
