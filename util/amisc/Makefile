# @(#)Makefile	1.1
EM	= ../..
BINDIR  = $(EM)/bin
h	= $(EM)/h
CFLAGS	= -n -O -I$h
ALL	= anm asize astrip aal
OFILES	= anm.o asize.o astrip.o aal.o
CFILES	= anm.c asize.c astrip.c aal.c

all:	$(ALL)

anm:	anm.c
	$(CC) $(CFLAGS) -o anm anm.c
asize:	asize.c
	$(CC) $(CFLAGS) -o asize asize.c
astrip: astrip.c
	$(CC) $(CFLAGS) -o astrip astrip.c
aal:	aal.c
	$(CC) $(CFLAGS) -DAUTORAN -o aal aal.c

install:	all
	for i in $(ALL); do cp $$i $(BINDIR)/$$i; done

cmp:	all
	for i in $(ALL); do cmp $$i $(BINDIR)/$$i; done

clean:	; rm -f $(ALL) $(OFILES)

pr:	$(CFILES)
	@pr -n Makefile $(CFILES)

opr:
	make pr | opr
