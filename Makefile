CC			= gcc
CFLAGS		= -Wall ${DEBUGFLAG}
OPTFLAG		= -O2
DEBUGFLAG	= -g
LINKER		= gcc
LFLAGS		= -lpthread

SRC = \
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
