# $Header$

PREFLAGS=-I../../h
CFLAGS=$(PREFLAGS)
LDFLAGS=-i
LINTOPTS=-hbxac $(PREFLAGS)
LIBS=../../lib/em_data.a
# LEXLIB is system dependent, try -ll or -lln first
LEXLIB=-lln

cgg:	bootgram.o
	cc $(LDFLAGS) bootgram.o $(LIBS) $(LEXLIB) -o cgg

bootgram.c:	bootgram.y
	@echo expect 1 shift/reduce conflict
	yacc bootgram.y
	mv y.tab.c bootgram.c

install:	cgg
	cp cgg ../../lib/cgg

cmp:		cgg
	cmp cgg ../../lib/cgg

lint:	bootgram.c
	lint $(LINTOPTS) bootgram.c
clean:
	rm -f bootgram.o bootgram.c bootlex.c cgg
bootgram.o:	bootlex.c
bootgram.o:	../../h/cg_pattern.h
