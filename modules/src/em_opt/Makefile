# $Header$
EMHOME = ../../..
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare
LIBOPT = libopt.a

CSRC = nopt.c aux.c mkcalls.c pseudo.c outputdfa.c outcalls.c\
		findworst.c initlex.c

SRCS = Makefile nopt.h parser.h parser.g syntax.l patterns $(CSRC)

NOFILES=nopt.o dfa.o trans.o incalls.o pseudo.o aux.o mkcalls.o

POFILES=parser.o syntax.o outputdfa.o outcalls.o findworst.o initlex.o Lpars.o 

GENFILES=Lpars.h Lpars.c parserdummy parser.c syntax.c dfadummy\
		dfa.c dfa.c.save trans.c trans.c.save incalls.c incalls.c.save

all:		$(LIBOPT)

install:	all
		$(INSTALL) lib/$(LIBOPT)

cmp:		all
		$(COMPARE) lib/$(LIBOPT)

pr:
		@pr $(SRCS)

opr:
		make pr | opr

clean:
		rm -f $(NOFILES) $(POFILES) $(GENFILES) parser core makerror

# set HOWMUCH to head -20 to limit number of patterns used
#HOWMUCH=head -20
HOWMUCH=cat

LEXLIB=-ll
INCLDIR=-I$(EMHOME)/h -I$(EMHOME)/modules/h -I$(EMHOME)/modules/pkg
PREFLAGS=$(INCLDIR) -DPRIVATE=static
PROFFLAG=-O
CFLAGS=$(PREFLAGS) $(PROFFLAG)
LLOPT=

$(LIBOPT):	dfadummy $(NOFILES)
		rm -f $(LIBOPT)
		ar rc $(LIBOPT) $(NOFILES)
		-sh -c 'ranlib $(LIBOPT)'

dfadummy:	patterns parser
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

# How to build program to parse patterns table and build c files.

PARSERLIB=$(EMHOME)/lib/em_data.a\
		$(EMHOME)/modules/lib/libprint.a\
		$(EMHOME)/modules/lib/liballoc.a\
		$(EMHOME)/modules/lib/libstring.a\
		$(EMHOME)/modules/lib/libsystem.a

parser:		parserdummy $(POFILES) $(PARSERLIB)
		$(CC) -o parser $(LDFLAGS) $(POFILES) $(PARSERLIB) $(LEXLIB)

parserdummy:	parser.g
		LLgen $(LLOPT) parser.g
		touch parserdummy

nopt.o:		nopt.h
dfa.o:		nopt.h
aux.o:		nopt.h
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
