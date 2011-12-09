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
			pcontrol.c \
			list.c \
			net.c \
			pthreads.c \
			jpeg_xdr.c \
			jpeg_clnt.c \
			proxy.c


SOBJS = $(SSRC:.c=.o)
COBJS = $(CSRC:.c=.o)
POBJS = $(PSRC:.c=.o)

all: proxy compress

proxy: $(POBJS)
	$(LINKER) $(LFLAGS) $(POBJS) -o $@

compress: 
	$(LINKER) jpeg_shrink.c jpeg_xdr.c jpeg_svc.c -o $@ jpeg-6b/lowres.o jpeg-6b/lowres-write.o jpeg-6b/libjpeg.a

clean:
	rm $(SOBJS) $(COBJS) $(POBJS) client webserver proxy compress
