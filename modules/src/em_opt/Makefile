# $Header$
EMHOME=/usr/em
MODLIB=$(EMHOME)/modules/lib
PARSERLIB=$(EMHOME)/lib/em_data.a $(MODLIB)/libprint.a $(MODLIB)/liballoc.a\
	$(MODLIB)/libstring.a $(MODLIB)/libsystem.a
HOWMUCH=head -10
LEXLIB=-ll
INCLDIR=-I$(EMHOME)/h -I$(EMHOME)/modules/h -I$(EMHOME)/modules/pkg
PREFLAGS=$(INCLDIR) -DPRIVATE=static -DDEBUG
PROFFLAG=-O
CFLAGS=$(PREFLAGS) $(PROFFLAG)
LLOPT=
GFILES=parser.g
POFILES=parser.o syntax.o outputdfa.o outcalls.o findworst.o outputrepl.o\
	initlex.o Lpars.o 
NOFILES=nopt.o dfa.o trans.o incalls.o psuedo.o aux.o mkcalls.o
GENFILES=Lpars.h Lpars.c parserdummy parser.c syntax.c dfadummy\
	dfa.c dfa.c.save trans.c trans.c.save incalls.c incalls.c.save

all:	libopt.a

libopt.a:	dfadummy $(NOFILES)
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

parser: parserdummy $(POFILES) $(PARSERLIB)
	$(CC) -o parser $(LDFLAGS) $(POFILES) $(PARSERLIB) $(LEXLIB)

parserdummy:	$(GFILES)
	LLgen $(LLOPT) $(GFILES)
	touch parserdummy

clean:
	rm -f $(NOFILES) $(POFILES) $(GENFILES) parser

nopt.o:	nopt.h
dfa.o:	nopt.h
aux.o:	nopt.h
trans.o:	nopt.h
psuedo.o:	nopt.h
incalls.o:	nopt.h
mkcalls.o:	nopt.h

parser.o:	Lpars.h parser.h
Lpars.o:	Lpars.h
syntax.o:	syntax.l parser.h Lpars.h
outputdfa.o:	parser.h Lpars.h
outcalls.o:	parser.h
findworst.o:	parser.h
outputrepl.o:	parser.h Lpars.h
initlex.o:	parser.h
