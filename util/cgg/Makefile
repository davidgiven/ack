# $Header$

PREFLAGS=-I../../h
CFLAGS=$(PREFLAGS)
LDFLAGS=
LINTOPTS=-hbxac $(PREFLAGS)
LIBS=../../lib/em_data.a
# LEXLIB is system dependent, try -ll or -lln first
LEXLIB = -lln

cgg:	bootgram.o main.o bootlex.o
	$(CC) $(LDFLAGS) bootgram.o main.o bootlex.o $(LIBS) $(LEXLIB) -o cgg

bootgram.c:	bootgram.y
	@echo expect 1 shift/reduce conflict
	yacc -d bootgram.y
	mv y.tab.c bootgram.c

install:	cgg
	cp cgg ../../lib/cgg

cmp:		cgg
	cmp cgg ../../lib/cgg

lint:	bootgram.c main.c bootlex.c
	lint $(LINTOPTS) bootgram.c main.c bootlex.c
clean:
	rm -f *.o bootgram.c bootlex.c cgg y.tab.h
bootgram.o:	booth.h
bootgram.o:	../../h/cg_pattern.h
bootlex.o:	booth.h
bootlex.o:	../../h/cg_pattern.h
main.o:	booth.h
main.o:	../../h/cg_pattern.h
