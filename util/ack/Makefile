# $Header$
EMHOME=../..
HFILES=ack.h list.h trans.h data.h dmach.h grows.h
DSRC=list.c data.c main.c scan.c svars.c trans.c util.c rmach.c run.c grows.c\
     files.c
ISRC=dmach.c intable.c
OBJ=list.o data.o main.o scan.o svars.o trans.o util.o rmach.o run.o \
    dmach.o intable.o grows.o files.o
ACKDIR=$(EMHOME)/lib
FE=fe
INTABLES=pdp
LNTABLES=acc apc abc ocm m2 vax4 i86 i386 m68k2 m68k4 pmds pmds4 mantra \
	m68020 z8000 em22 em24 em44 6500 6800 6805 6809 i80 ns s2650 z80 \
	sun3 sun2 xenix3 minix minixST
INCLUDES=-I$(EMHOME)/h
CFLAGS=-O $(INCLUDES)
LDFLAGS=
BINDIR=$(EMHOME)/bin
MANDIR=$(EMHOME)/man
MODDIR=$(EMHOME)/modules/lib

head:   ack ack.1

install:	ack ack.1
		rm -f $(BINDIR)/ack
		cp ack $(BINDIR)/ack
		-cd $(BINDIR) ; \
		for i in $(INTABLES) $(LNTABLES) ; do rm -f $$i ; ln ack $$i ; done
		rm -f $(MANDIR)/ack.1
		cp ack.1 $(MANDIR)/ack.1

cmp:		ack ack.1
		-cmp ack $(BINDIR)/ack
		-cmp ack.1 $(MANDIR)/ack.1

ack.1:		ack.1.X
		-sh -c 'tbl < ack.1.X > ack.1'
		-sh -c 'if test -s ack.1 ; then : ; else cp ack.1.X ack.1 ; fi'

clean:
		-rm -f *.old *.o ack ack.1

ack:    $(OBJ)
	$(CC) $(LDFLAGS) -o ack $(OBJ) $(MODDIR)/libstring.a

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
	$(CC) -o mktables mktables.c

pr:
	@pr Makefile $(HFILES) $(DSRC)

opr:
	make pr | opr

lint:   $(ISRC)
	lint -hbx $(INCLUDES) $(DSRC) $(ISRC)
