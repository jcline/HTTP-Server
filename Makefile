CC			= gcc
#CFLAGS		= -Wall -std=gnu99 -fstrict-aliasing ${DEBUGFLAG} -DDEBUG
CFLAGS		= -Wall -std=gnu99 -fstrict-aliasing ${OPTFLAG} -DNDEBUG
OPTFLAG		= -O2
DEBUGFLAG	= -g3 -ggdb 
LINKER		= gcc
LFLAGS		= -lpthread

SSRC = \
			list.c \
			sthreads.c \
			rthreads.c \
			scontrol.c \
			rcontrol.c \
			log.c \
			main.c

CSRC= \
			list.c \
			cthreads.c \
			rthreads.c \
			ccontrol.c \
			rcontrol.c \
			client.c


SOBJS = $(SSRC:.c=.o)
COBJS = $(CSRC:.c=.o)

all: webserver client

webserver: $(SOBJS)
	$(LINKER) $(LFLAGS) $(SOBJS) -o $@

client: $(COBJS)
	$(LINKER) $(LFLAGS) $(COBJS) -o $@

clean:
	rm $(SOBJS) $(COBJS) client webserver
