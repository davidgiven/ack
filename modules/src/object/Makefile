EMHOME = ../../..
MODULES = $(EMHOME)/modules
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
INCLUDES = -I$(EMHOME)/h
CFLAGS = -O $(INCLUDES) $(COPT)
AR = ar
SUF = o
LIBSUF = a
RCC = cc	# must produce a runnable object

CFILES =	rd_arhdr.c wr_arhdr.c \
		rd_ranlib.c wr_ranlib.c \
		rd_bytes.c wr_bytes.c \
		rd.c wr.c \
		wr_putc.c \
		rd_int2.c wr_int2.c \
		rd_unsig2.c \
		rd_long.c wr_long.c
# do not change the order in OFILES
OFILES =	rd.$(SUF) rd_arhdr.$(SUF) rd_int2.$(SUF) rd_long.$(SUF) rd_ranlib.$(SUF) rd_unsig2.$(SUF) \
		rd_bytes.$(SUF) wr_arhdr.$(SUF) wr_int2.$(SUF) wr_long.$(SUF) wr_putc.$(SUF) wr.$(SUF) \
		wr_ranlib.$(SUF) wr_bytes.$(SUF)

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		libobject.$(LIBSUF)

install:	all
		$(INSTALL) lib/libobject.$(LIBSUF)
		$(INSTALL) man/object.3

compare:	all
		-$(COMPARE) lib/libobject.$(LIBSUF)
		-$(COMPARE) man/object.3

pr:
		@pr Makefile byte_order.c object.h $(CFILES)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF) nohup.out Out byte_order byte_order.h

libobject.$(LIBSUF):	$(OFILES)
		$(AR) r libobject.$(LIBSUF) $(OFILES)
		-sh -c 'ranlib libobject.$(LIBSUF)'

depend:		byte_order.h
		sed '/^#AUTOAUTO/,$$d' Makefile > Makefile.new
		echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
		mkdep $(CFILES) | sed 's/\.c:/\.$(SUF):/' >> Makefile.new
		mv Makefile Makefile.$(SUF)ld
		mv Makefile.new Makefile

byte_order:	byte_order.c
		$(RCC) -o byte_order byte_order.c

byte_order.h:	byte_order
		byte_order > byte_order.h

lintlib:	byte_order.h
		lint $(INCLUDES) -Cobject $(CFILES)
		mv llib-lobject.ln $(MODULES)/lib
		
#AUTOAUTOAUTOAUTOAUTOAUTOAUTO
rd_arhdr.$(SUF): byte_order.h object.h
wr_arhdr.$(SUF): byte_order.h object.h
rd_ranlib.$(SUF): byte_order.h object.h
wr_ranlib.$(SUF): byte_order.h object.h
rd.$(SUF): byte_order.h object.h
wr.$(SUF): byte_order.h object.h
rd_int2.$(SUF): byte_order.h object.h
wr_int2.$(SUF): byte_order.h object.h
rd_unsig2.$(SUF): byte_order.h object.h
rd_long.$(SUF): byte_order.h object.h
wr_long.$(SUF): byte_order.h object.h
