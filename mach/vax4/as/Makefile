# $Header$
EMHOME	= ../../..
h	= $(EMHOME)/h
LIBOBJ	= $(EMHOME)/modules/lib/libobject.a
CDIR	= $(EMHOME)/mach/proto/as
CPP	= $(EMHOME)/lib/cpp
DEF	=

FFLAG	=
INCL	= -I. -I$h $(DEF)
CFLAGS	= $(FFLAG) -O $(INCL)
YFLAGS	= -d
LDFLAGS	= $(FFLAG)

CSRC	= $(CDIR)/comm3.c $(CDIR)/comm4.c $(CDIR)/comm5.c \
	  $(CDIR)/comm6.c $(CDIR)/comm7.c $(CDIR)/comm8.c
COBJ	= comm3.o comm4.o comm5.o \
	  comm6.o comm7.o comm8.o
MACH	= mach0.c mach1.c mach2.c mach3.c mach4.c mach5.c
COMM	= $(CDIR)/comm0.h $(CDIR)/comm1.h $(CDIR)/comm2.y $(CSRC)

all:	as

install:all
	../../install as

cmp:
	-../../compare as

clean:
	rm -f *.o as as.[cy] y.tab.h

pr:	$(MACH)
	@pr -n $(MACH)

opr:
	make pr | opr
as:	$(COBJ) as.o
	$(CC) $(LDFLAGS) $(COBJ) as.o $(LIBOBJ) -o as

as.y:	$(CDIR)/comm2.y
	$(CPP) -P -I. -I$h $(DEF) $(CDIR)/comm2.y >as.y
	-@if test -f Expect ; then cat Expect ; else echo "expect 1 shift/reduce conflict"; fi

lint:   $(CSRC) as.c
	lint $(INCL) $(CSRC) as.c

y.tab.h:	as.c
$(COBJ):	y.tab.h
$(COBJ) as.y:	$(CDIR)/comm0.h mach0.c
$(COBJ) as.y:	$(CDIR)/comm1.h mach1.c
as.y:		mach2.c
comm3.o:	mach3.c
as.y:		mach4.c
comm8.o:	mach5.c
comm3.o:	$(CDIR)/comm3.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm3.c
comm4.o:	$(CDIR)/comm4.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm4.c
comm5.o:	$(CDIR)/comm5.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm5.c
comm6.o:	$(CDIR)/comm6.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm6.c
comm7.o:	$(CDIR)/comm7.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm7.c
comm8.o:	$(CDIR)/comm8.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm8.c
