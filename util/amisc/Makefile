EM	= ../..
BINDIR  = $(EM)/bin
MANDIR	= $(EM)/man
LIBDIR	= $(EM)/modules/lib
h	= $(EM)/h
LDFLAGS = 
CFLAGS	= $(LDFLAGS) -O -I$h
ALL	= anm asize astrip
OFILES	= anm.o asize.o astrip.o
CFILES	= anm.c asize.c astrip.c
LIBS	= $(LIBDIR)/libobject.a

all:	$(ALL)

anm:	anm.c
	$(CC) $(CFLAGS) -o anm anm.c $(LIBS)
asize:	asize.c
	$(CC) $(CFLAGS) -o asize asize.c $(LIBS)
astrip: astrip.c
	$(CC) $(CFLAGS) -o astrip astrip.c $(LIBS)
#not installed:
ashow:	ashow.c
	$(CC) $(CFLAGS) -o ashow ashow.c $(LIBS)

install:	all
	for i in $(ALL); do rm -f $(BINDIR)/$$i; cp $$i $(BINDIR)/$$i; done
	for i in anm.1 asize.1 astrip.1; do rm -f $(MANDIR)/$$i; cp $$i $(MANDIR)/$$i; done

cmp:	all
	-for i in $(ALL); do cmp $$i $(BINDIR)/$$i; done
	-for i in anm.1 astrip.1 asize.1 ; do cmp $$i $(MANDIR)/$$i; done

clean:	; rm -f $(ALL) $(OFILES)

pr:
	@pr `pwd`/Makefile `pwd`/anm.c `pwd`/astrip.c `pwd`/asize.c

opr:
	make pr | opr
