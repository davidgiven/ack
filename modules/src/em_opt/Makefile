# $Header$

# Where to find em home directory

EMHOME=/usr/em
MODLIB=$(EMHOME)/modules/lib

# set HOWMUCH to head -20 to limit number of patterns used
HOWMUCH=cat

LEXLIB=-ll
INCLDIR=-I$(EMHOME)/h -I$(EMHOME)/modules/h -I$(EMHOME)/modules/pkg -I../h
PREFLAGS=$(INCLDIR) -DPRIVATE=static
PROFFLAG=-O
CFLAGS=$(PREFLAGS) $(PROFFLAG)
LLOPT=

GENFILES=Lpars.h Lpars.c parserdummy parser.c syntax.c dfadummy\
	dfa.c dfa.c.save trans.c trans.c.save incalls.c incalls.c.save

all:	libopt.a

NOFILES=nopt.o dfa.o trans.o incalls.o pseudo.o aux.o mkcalls.o

libopt.a:	dfadummy $(NOFILES)
	rm -f libopt.a
	ar rc libopt.a $(NOFILES)
	ranlib libopt.a

dfadummy: patterns parser
	-mv dfa.c dfa.c.save
	-mv trans.c trans.c.save
	-mv incalls.c incalls.c.save
	-/lib/cpp patterns | $(HOWMUCH) >/tmp/patts
	parser </tmp/patts
	-rm /tmp/patts
	-if cmp -s dfa.c dfa.c.save; then mv dfa.c.save dfa.c; else exit 0; fi
	-if cmp -s trans.c trans.c.save; then mv trans.c.save trans.c; else exit 0; fi
	-if cmp -s incalls.c incalls.c.save; then mv incalls.c.save incalls.c; else exit 0; fi
	touch dfadummy

POFILES=parser.o syntax.o outputdfa.o outcalls.o findworst.o initlex.o Lpars.o 

PARSERLIB=$(EMHOME)/lib/em_data.a $(MODLIB)/libprint.a $(MODLIB)/liballoc.a\
	$(MODLIB)/libstring.a $(MODLIB)/libsystem.a

parser: parserdummy $(POFILES) $(PARSERLIB)
	$(CC) -o parser $(LDFLAGS) $(POFILES) $(PARSERLIB) $(LEXLIB)

GFILES=parser.g

parserdummy:	$(GFILES)
	LLgen $(LLOPT) $(GFILES)
	touch parserdummy

clean:
	rm -f $(NOFILES) $(POFILES) $(GENFILES) parser

nopt.o:	nopt.h
dfa.o:	nopt.h
aux.o:	nopt.h
trans.o:	nopt.h
pseudo.o:	nopt.h
incalls.o:	nopt.h
mkcalls.o:	nopt.h

parser.o:	Lpars.h parser.h
Lpars.o:	Lpars.h
syntax.o:	syntax.l parser.h Lpars.h
outputdfa.o:	parser.h Lpars.h
outcalls.o:	parser.h
findworst.o:	parser.h
initlex.o:	parser.h
