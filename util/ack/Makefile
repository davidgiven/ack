# $Header$
HFILES=ack.h list.h trans.h data.h dmach.h grows.h
DSRC=list.c data.c main.c scan.c svars.c trans.c util.c rmach.c run.c grows.c\
     files.c
ISRC=dmach.c intable.c
OBJ=list.o data.o main.o scan.o svars.o trans.o util.o rmach.o run.o \
    dmach.o intable.o grows.o files.o
ACKDIR=../../lib/ack
FE=fe
INTABLES=pdp int
LNTABLES=acc apc abc vax2 vax4 i86 ix m68k2 m68k4 pmds z8000 int22 int24 int44\
6500 6800 6809 8080 nascom ns s2650 z80 
CFLAGS=-O -n
BINDIR=../../bin

head:   ack

install:	ack
		cp ack $(BINDIR)/ack
		-cd $(BINDIR) ; \
		for i in $(INTABLES) $(LNTABLES) ; do ln ack $$i ; done
		(cd pc ; make install )

cmp:		ack
		cmp ack $(BINDIR)/ack
		(cd pc ; make cmp )

clean:
		-rm -f *.old *.o ack
		(cd pc ; make clean )

ack:    $(OBJ)
	$(CC) -o ack $(CFLAGS) $(OBJ)

grows.o files.o list.o run.o \
data.o main.o scan.o trans.o rmach.o util.o : ack.h list.h

files.o data.o main.o scan.o run.o trans.o rmach.o: trans.h data.h

files.o rmach.o trans.o grows.c : grows.h

rmach.c: dmach.h

files.o main.o rmach.o : ../../h/em_path.h

main.o : ../../h/local.h

malloc.o svars.o:	ack.h

dmach.c intable.c: mktables dmach.h
	: mktables $(ACKDIR) # $(FE) $(INTABLES)
	mktables $(ACKDIR)

mktables:       mktables.c
	cc -o mktables mktables.c

pr:
	@pr Makefile $(HFILES) $(DSRC) $(ACKDIR)/*
	@(cd pc ; make pr)

opr:
	make pr | opr

lint:   $(ISRC)
	lint -hbx $(DSRC) $(ISRC)
