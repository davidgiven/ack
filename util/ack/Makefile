# $Header$
EMHOME=../..
HFILES=ack.h list.h trans.h data.h dmach.h grows.h
DSRC=list.c data.c main.c scan.c svars.c trans.c util.c rmach.c run.c grows.c\
     files.c
ISRC=dmach.c intable.c
OBJ=list.o data.o main.o scan.o svars.o trans.o util.o rmach.o run.o \
    dmach.o intable.o grows.o files.o
ACKDIR=$(EMHOME)/lib/descr
FE=fe
INTABLES=pdp int
LNTABLES=acc apc abc vax4 i86 m68k2 m68k4 pmds pmds4 mantra mantra4 \
	m68020 z8000 int22 int24 int44 6500 6800 6805 6809 i80 ns s2650 z80 \
	sun3
INCLUDES=-I$(EMHOME)/h
CFLAGS=-O -n $(INCLUDES)
BINDIR=$(EMHOME)/bin
MANDIR=$(EMHOME)/man
MODDIR=$(EMHOME)/modules/lib

head:   ack

install:	ack
		rm -f $(BINDIR)/ack
		cp ack $(BINDIR)/ack
		-cd $(BINDIR) ; \
		for i in $(INTABLES) $(LNTABLES) ; do rm -f $$i ; ln ack $$i ; done
		(cd pc ; make install )
		rm -f $(MANDIR)/man/ack.1
		cp ack.1 $(MANDIR)/man/ack.1

cmp:		ack
		-cmp ack $(BINDIR)/ack
		(cd pc ; make cmp )
		-cmp ack.1 $(MANDIR)/ack.1

clean:
		-rm -f *.old *.o ack
		(cd pc ; make clean )

ack:    $(OBJ)
	$(CC) -o ack $(CFLAGS) $(OBJ) $(MODDIR)/libstring.a

grows.o files.o list.o run.o \
data.o main.o scan.o trans.o rmach.o util.o : ack.h list.h

files.o data.o main.o scan.o run.o trans.o rmach.o: trans.h data.h

files.o rmach.o trans.o grows.c : grows.h

rmach.c: dmach.h

files.o main.o rmach.o : $(EMHOME)/h/em_path.h

main.o : $(EMHOME)/h/local.h

svars.o:	ack.h

dmach.c intable.c: mktables dmach.h
	: mktables $(ACKDIR) # $(FE) $(INTABLES)
	mktables $(ACKDIR)

mktables:       mktables.c
	cc -o mktables mktables.c

pr:
	@pr Makefile $(HFILES) $(DSRC)
	@(cd pc ; make pr)

opr:
	make pr | opr

lint:   $(ISRC)
	lint -hbx $(DSRC) $(ISRC)
