# @(#)Makefile	1.1
EM	= ../..
h	= $(EM)/h
CFLAGS	= -O -I$h
ALL	= anm asize astrip
OFILES	= anm.o asize.o astrip.o
CFILES	= anm.c asize.c astrip.c

all:	$(ALL)

anm:	anm.c
	$(CC) $(CFLAGS) anm.c -o anm

asize:	asize.c
	$(CC) $(CFLAGS) asize.c -o asize

astrip:	astrip.c
	$(CC) $(CFLAGS) astrip.c -o astrip

clean:
	rm -f $(ALL) $(OFILES)

print:	$(CFILES)
	pr -n Makefile $(CFILES) | lpr
