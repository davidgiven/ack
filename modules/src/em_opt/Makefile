# $Header$
EMHOME = ../../..
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare
LIBDIR	= $(EMHOME)/modules/lib
BINDIR	= $(EMHOME)/lib
LIBOPT = libemopt.a

# libraries required for standalone em_nopt
INK	= $(LIBDIR)/libread_emk.a
OUTK	= $(LIBDIR)/libemk.a
SYS	= $(LIBDIR)/libsystem.a
ALLOC	= $(LIBDIR)/liballoc.a $(LIBDIR)/malloc.o
DATA	= $(EMHOME)/lib/em_data.a
LIBS	= $(INK) $(OUTK) $(ALLOC) $(DATA) $(SYS)

# set HOWMUCH to head -20 to limit number of patterns used
#HOWMUCH= head -20
HOWMUCH	= cat

LEXLIB	= -ll
INCLDIR	= -I$(EMHOME)/h -I$(EMHOME)/modules/h -I$(EMHOME)/modules/pkg
PREFLAGS= $(INCLDIR) -DPRIVATE=static#	 -DDEBUG

# Enable the next line to produce a version that output's the line number
# from the patterns file each time an optimization is performed.
#PREFLAGS= $(PREFLAGS) -DSTATS
PROFFLAG= -O
CFLAGS	= $(PREFLAGS) $(PROFFLAG)
LLOPT	=
CMD	= '$(CC) -c $(CFLAGS)'

.SUFFIXES:	.d .r

.r.d:; CMD=$(CMD); export CMD; awk -f makefuns.awk $*.r | sh
		touch $@

CSRC = main.c nopt.c mkstrct.c aux.c outputdfa.c outcalls.c\
		findworst.c initlex.c

SRCS = Makefile nopt.h parser.h parser.g syntax.l pseudo.r patterns $(CSRC)

NOFILES = nopt.o dfa.o trans.o aux.o

POFILES = parser.o syntax.o outputdfa.o outcalls.o findworst.o initlex.o Lpars.o

GENFILES = Lpars.h Lpars.c parserdummy parser.c syntax.c dfadummy\
		dfa.c dfa.c.new trans.h trans.h.new trans.c trans.c.new\
		incalls.d incalls.r incalls.r.new pseudo.d

all:		em_nopt $(LIBOPT)

install:	all
		$(INSTALL) lib/$(LIBOPT)
		cp em_nopt $(BINDIR)

cmp:		all
		$(COMPARE) lib/$(LIBOPT)

pr:
		@pr $(SRCS)

opr:
		make pr | opr

clean:
		rm -f O_*.o
		rm -f O_*.c
		rm -f $(NOFILES) main.o mkstrct.o $(POFILES)
		rm -f $(GENFILES) parser em_nopt $(LIBOPT)

em_nopt:	main.o $(NOFILES)
		$(CC) -o em_nopt main.o $(NOFILES) $(LIBS)

$(LIBOPT):	$(NOFILES) mkstrct.o pseudo.d incalls.d
		rm -f $(LIBOPT)
		ar rc $(LIBOPT) O_*.o $(NOFILES) mkstrct.o
		-sh -c 'ranlib $(LIBOPT)'

dfadummy:	patterns parser
		-/lib/cpp patterns | $(HOWMUCH) >/tmp/patts
		parser </tmp/patts
		-rm /tmp/patts
		touch dfadummy

# How to build program to parse patterns table and build c files.

PARSERLIB = $(EMHOME)/lib/em_data.a\
		$(EMHOME)/modules/lib/liballoc.a\
		$(EMHOME)/modules/lib/libstring.a\
		$(EMHOME)/modules/lib/libsystem.a

parser:		parserdummy $(POFILES) $(PARSERLIB)
		$(CC) -o parser $(LDFLAGS) $(POFILES) $(PARSERLIB) $(LEXLIB)

parserdummy:	parser.g
		LLgen $(LLOPT) parser.g
		touch parserdummy

main.o:		nopt.h
nopt.o:		nopt.h
mkstrct.o:	nopt.h
aux.o:		nopt.h
pseudo.d:	nopt.h makefuns.awk
dfa.o:		nopt.h trans.h dfadummy
trans.o:	nopt.h dfadummy
incalls.d:	nopt.h makefuns.awk
incalls.r:	dfadummy

parser.o:	Lpars.h parser.h
Lpars.o:	Lpars.h
syntax.o:	syntax.l parser.h Lpars.h
outputdfa.o:	parser.h Lpars.h
outcalls.o:	parser.h
findworst.o:	parser.h
initlex.o:	parser.h
