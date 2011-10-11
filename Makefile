CC			= gcc
#CFLAGS		= -Wall -std=gnu99 ${DEBUGFLAG} -DDEBUG
CFLAGS		= -Wall -std=gnu99 ${OPTFLAG} -DNDEBUG
OPTFLAG		= -O2
DEBUGFLAG	= -g3 -ggdb 
LINKER		= gcc
LFLAGS		= -lpthread

SSRC = \
			hash.c \
			list.c \
			sthreads.c \
			rthreads.c \
			scontrol.c \
			rcontrol.c \
			log.c \
			main.c

CSRC= \
			hash.c \
			list.c \
			cthreads.c \
			rthreads.c \
			ccontrol.c \
			rcontrol.c \
			client.c

PSRC = \
			hash.c \
			list.c \
			sthreads.c \
			scontrol.c \
			rcontrol.c \
			log.c \
			proxy.c


SOBJS = $(SSRC:.c=.o)
COBJS = $(CSRC:.c=.o)
POBJS = $(PSRC:.c=.o)

all: webserver client proxy

webserver: $(SOBJS)
	$(LINKER) $(LFLAGS) $(SOBJS) -o $@

client: $(COBJS)
	$(LINKER) $(LFLAGS) $(COBJS) -o $@

proxy: $(POBJS)
	$(LINKER) $(LFLAGS) $(POBJS) -o $@

clean:
	rm $(SOBJS) $(COBJS) $(POBJS) client webserver proxy
