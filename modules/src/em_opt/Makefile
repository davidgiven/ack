# $Header$
EMHOME =	../../..
INSTALL =	$(EMHOME)/modules/install
COMPARE =	$(EMHOME)/modules/compare
LINT	=	lint
BINDIR	=	$(EMHOME)/lib
LIBOPT =	libemopt.a

# set HOWMUCH to head -20 to limit number of patterns used
#HOWMUCH =	head -20
HOWMUCH	=	cat

LEXLIB =	-ll
INCLDIR	=	-I$(EMHOME)/h -I$(EMHOME)/modules/h -I$(EMHOME)/modules/pkg
PREFLAGS =	$(INCLDIR) -DPRIVATE=static#	 -DDEBUG

# Enable the next line to produce a version that output's the line number
# from the patterns file each time an optimization is performed.
#PREFLAGS =	$(PREFLAGS) -DSTATS

PROFFLAG =	-O
CFLAGS =	$(PREFLAGS) $(PROFFLAG)
LINTFLAGS =	$(PREFLAGS) -DNORCSID
LLOPT =
CMD =		'$(CC) -c $(CFLAGS)'

.SUFFIXES:	.d .r

.r.d:		; CMD=$(CMD); export CMD; awk -f makefuns.awk $*.r | sh
		touch $@

CSRC =		main.c nopt.c mkstrct.c aux.c outputdfa.c outcalls.c\
		findworst.c initlex.c

SRCS =		Makefile nopt.h parser.h parser.g syntax.l pseudo.r\
		patterns $(CSRC)

NOFILES =	nopt.o mkstrct.o dfa.o trans.o aux.o

POFILES =	parser.o syntax.o outputdfa.o outcalls.o findworst.o\
		initlex.o Lpars.o

GENFILES =	Lpars.h Lpars.c parserdummy parser.c syntax.c dfadummy\
		dfa.c dfa.c.new trans.c trans.c.new\
		incalls.d incalls.r incalls.r.new pseudo.d

all:		em_nopt $(LIBOPT) em_opt.3

install:	all
		$(INSTALL) lib/$(LIBOPT)
		$(INSTALL) man/em_opt.3
		cp em_nopt.6 $(EMHOME)/man
		cp em_nopt $(BINDIR)/em_nopt

cmp:		all
		$(COMPARE) lib/$(LIBOPT)
		$(COMPARE) man/em_opt.3
		-cmp em_nopt $(BINDIR)/em_nopt

pr:
		@pr $(SRCS)

opr:
		make pr | opr

lint:		lintparser lintnopt

clean:
		rm -f O_*.o
		rm -f O_*.c
		rm -f $(NOFILES) main.o mkstrct.o $(POFILES)
		rm -f $(GENFILES) parser em_nopt $(LIBOPT)

# How to build stand alone version of the optimizer

NOPTLIB = 	$(EMHOME)/modules/lib/libread_emk.a\
		$(EMHOME)/modules/lib/libemk.a\
		$(EMHOME)/modules/lib/liballoc.a\
		$(EMHOME)/modules/lib/malloc.o\
		$(EMHOME)/modules/lib/libsystem.a\
		$(EMHOME)/lib/em_data.a

em_nopt:	dfadummy main.o $(NOFILES)
		$(CC) -o em_nopt main.o $(NOFILES) $(NOPTLIB)

OLINT =		main.c mkstrct.c nopt.c aux.c dfa.c trans.c

OLINTLIB = 	$(EMHOME)/modules/lib/llib-lread_emkV.ln\
		$(EMHOME)/modules/lib/llib-lemk.ln\
		$(EMHOME)/modules/lib/llib-lsystem.ln\
		$(EMHOME)/modules/lib/llib-lalloc.ln

lintnopt:	dfadummy $(OLINT)
		$(LINT) $(LINTFLAGS) $(OLINT) $(OLINTLIB)

# How to build the library version of the optimizer

$(LIBOPT):	dfadummy $(NOFILES) mkstrct.o pseudo.d incalls.d
		rm -f $(LIBOPT)
		ar rc $(LIBOPT) O_*.o $(NOFILES) mkstrct.o
		-sh -c 'ranlib $(LIBOPT)'

dfadummy:	patterns parser
		-/lib/cpp patterns | $(HOWMUCH) >/tmp/patts
		parser </tmp/patts
		-rm /tmp/patts
		touch dfadummy

# How to build the lint library for the optimizer

LLINT =		nopt.c mkstrct.c aux.c dfa.c trans.c

lintlib:	dfadummy $(LLINT)
		$(LINT) $(LINTFLAGS) $(LLINT) -Cemopt
		mv llib-lemopt.ln $(EMHOME)/modules/lib

# How to build program to parse patterns table and build c files.

PARSERLIB =	$(EMHOME)/lib/em_data.a\
		$(EMHOME)/modules/lib/liballoc.a\
		$(EMHOME)/modules/lib/libstring.a\
		$(EMHOME)/modules/lib/libsystem.a

parser:		parserdummy $(POFILES) $(PARSERLIB)
		$(CC) -o parser $(LDFLAGS) $(POFILES) $(PARSERLIB) $(LEXLIB)

parserdummy:	parser.g
		LLgen $(LLOPT) parser.g
		touch parserdummy

PLINT =		parser.c syntax.c outputdfa.c outcalls.c\
		findworst.c initlex.c Lpars.c

PLINTLIB =	$(EMHOME)/modules/lib/llib-lalloc.ln\
		$(EMHOME)/modules/lib/llib-lstring.ln\
		$(EMHOME)/modules/lib/llib-lsystem.ln

lintparser:	parserdummy $(PLINT)
		$(LINT) $(LINTFLAGS) $(PLINT) $(PLINTLIB)

# Dependancies

main.o:		nopt.h
nopt.o:		nopt.h
mkstrct.o:	nopt.h
aux.o:		nopt.h
pseudo.d:	nopt.h makefuns.awk
dfa.o:		nopt.h
trans.o:	nopt.h
incalls.d:	nopt.h makefuns.awk

parser.o:	Lpars.h parser.h
Lpars.o:	Lpars.h
syntax.o:	syntax.l parser.h Lpars.h
outputdfa.o:	parser.h Lpars.h
outcalls.o:	parser.h
findworst.o:	parser.h
initlex.o:	parser.h
