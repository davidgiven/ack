EMHOME = ../../..
MODULES = $(EMHOME)/modules
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
INCLUDES = -I$(EMHOME)/h
CFLAGS = -O $(INCLUDES)
AR = ar
CFILES =	rd_arhdr.c wr_arhdr.c \
		rd_ranlib.c wr_ranlib.c \
		rd_bytes.c wr_bytes.c \
		rd.c wr.c \
		wr_putc.c \
		rd_int2.c wr_int2.c \
		rd_unsig2.c \
		rd_long.c wr_long.c
# do not change the order in OFILES
OFILES =	rd.o rd_arhdr.o rd_int2.o rd_long.o rd_ranlib.o rd_unsig2.o \
		rd_bytes.o wr_arhdr.o wr_int2.o wr_long.o wr_putc.o wr.o \
		wr_ranlib.o wr_bytes.o

all:		libobject.a

install:	all
		$(INSTALL) lib/libobject.a
		$(INSTALL) man/object.3

compare:	all
		$(COMPARE) lib/libobject.a
		$(COMPARE) man/object.3

pr:
		@pr Makefile byte_order.c object.h $(CFILES)

opr:
		make pr | opr

clean:
		rm -f *.[oa] nohup.out Out byte_order byte_order.h

libobject.a:	$(OFILES)
		$(AR) r libobject.a $(OFILES)
		-sh -c 'ranlib libobject.a'

depend:		byte_order.h
		sed '/^#AUTOAUTO/,$$d' Makefile > Makefile.new
		echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
		mkdep $(CFILES) | sed 's/\.c:/\.o:/' >> Makefile.new
		mv Makefile Makefile.old
		mv Makefile.new Makefile

byte_order:	byte_order.o
		$(CC) $(LDFLAGS) -o byte_order byte_order.o

byte_order.h:	byte_order
		byte_order > byte_order.h

lintlib:	byte_order.h
		lint $(INCLUDES) -Cobject $(CFILES)
		mv llib-lobject.ln $(MODULES)/lib
		
#AUTOAUTOAUTOAUTOAUTOAUTOAUTO
rd_arhdr.o: byte_order.h object.h
wr_arhdr.o: byte_order.h object.h
rd_ranlib.o: byte_order.h object.h
wr_ranlib.o: byte_order.h object.h
rd.o: byte_order.h object.h
wr.o: byte_order.h object.h
rd_int2.o: byte_order.h object.h
wr_int2.o: byte_order.h object.h
rd_unsig2.o: byte_order.h object.h
rd_long.o: byte_order.h object.h
wr_long.o: byte_order.h object.h
