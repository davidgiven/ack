# $Header$
EM	= ../../..
h	= $(EM)/h
CDIR	= $(EM)/mach/proto/nas
CPP	= $(EM)/lib/cpp
DEF	=

FFLAG	=
CFLAGS	= $(FFLAG) -O -I$h $(DEF)
YFLAGS	= -d
LDFLAGS	= $(FFLAG) -i

CSRC	= $(CDIR)/comm3.c $(CDIR)/comm4.c $(CDIR)/comm5.c \
	  $(CDIR)/comm6.c $(CDIR)/comm7.c $(CDIR)/comm8.c
COBJ	= $(CDIR)/comm3.o $(CDIR)/comm4.o $(CDIR)/comm5.o \
	  $(CDIR)/comm6.o $(CDIR)/comm7.o $(CDIR)/comm8.o
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
	$(CC) $(LDFLAGS) $(COBJ) as.o -o as

as.y:	$(CDIR)/comm2.y
	$(CPP) -P -I$h $(DEF) $(CDIR)/comm2.y >as.y
	@echo "expect 1 shift/reduce conflict"

lint:   $(CSRC) as.c
	lint $(CSRC) as.c

y.tab.h:	as.c
$(COBJ):	y.tab.h
$(COBJ) as.y:	$(CDIR)/comm0.h mach0.c
$(COBJ) as.y:	$(CDIR)/comm1.h mach1.c
as.y:		mach2.c
comm3.o:	mach3.c
as.y:		mach4.c
comm8.o:	mach5.c
