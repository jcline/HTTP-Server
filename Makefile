CC			= gcc
CFLAGS		= -Wall -std=gnu99 ${DEBUGFLAG} -DDEBUG
#CFLAGS		= -Wall -std=gnu99 ${OPTFLAG} -DNDEBUG
OPTFLAG		= -O3
DEBUGFLAG	= -g3 -ggdb 
LINKER		= gcc
LFLAGS		= -lpthread 

SSRC = \
			list.c \
			net.c \
			sthreads.c \
			scontrol.c \
			main.c

CSRC= \
			list.c \
			net.c \
			cthreads.c \
			ccontrol.c \
			client.c

PSRC = \
			list.c \
			net.c \
			pthreads.c \
			pcontrol.c \
			jpeg_clnt.c \
			jpeg_xdr.c \
			proxy.c


SOBJS = $(SSRC:.c=.o)
COBJS = $(CSRC:.c=.o)
POBJS = $(PSRC:.c=.o)

proxy: $(POBJS)
	$(LINKER) $(LFLAGS) $(POBJS) -o $@

compress: jpeg_xdr.o jpeg.h
	$(LINKER) jpeg_svc.c -o $@

clean:
	rm $(SOBJS) $(COBJS) $(POBJS) client webserver proxy compress
