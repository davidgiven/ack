# @(#)Makefile	1.1
EM	= ../..
h	= $(EM)/h
CFLAGS	= -n -O -I$h
ALL	= anm asize astrip
OFILES	= anm.o asize.o astrip.o
CFILES	= anm.c asize.c astrip.c

all:	$(ALL)

cp cmp:	all
	for i in $(ALL); do $@ $$i $$ROOT/usr/bin/$$i; done

clean:	; rm -f $(ALL) $(OFILES)
get:	; getall
unget:	; ungetall

print:	$(CFILES)
	pr -n Makefile $(CFILES) | lpr
