# $Header$
EMHOME =	../../..
INSTALL =	$(EMHOME)/modules/install
COMPARE =	$(EMHOME)/modules/compare
LINT	=	lint
BINDIR	=	$(EMHOME)/lib

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
CFLAGS =	$(PREFLAGS) $(PROFFLAG) $(COPT)
LINTFLAGS =	$(PREFLAGS) -DNORCSID
LLOPT =
CMD =		'$(CC) -c $(CFLAGS)'
AR = ar
SUF = o
LIBSUF = a

LIBOPT =	libemopt.$(LIBSUF)
LIBCEOPT =	libCEopt.$(LIBSUF)

.SUFFIXES:	.d .r

.r.d:		; CMD=$(CMD); export CMD; awk -f makefuns.awk $*.r | sh -x
		touch $@

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

CSRC =		main.c nopt.c mkstrct.c aux.c outputdfa.c outcalls.c\
		findworst.c initlex.c

SRCS =		Makefile nopt.h parser.h parser.g syntax.l pseudo.r\
		patterns $(CSRC)

NOFILES =	nopt.$(SUF) mkstrct.$(SUF) dfa.$(SUF) trans.$(SUF) aux.$(SUF)

POFILES =	parser.$(SUF) syntax.$(SUF) outputdfa.$(SUF) outcalls.$(SUF) findworst.$(SUF)\
		initlex.$(SUF) Lpars.$(SUF)

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
		-$(COMPARE) lib/$(LIBOPT)
		-$(COMPARE) man/em_opt.3
		-cmp em_nopt $(BINDIR)/em_nopt

pr:
		@pr $(SRCS)

opr:
		make pr | opr

lint:		lintparser lintnopt

clean:
		rm -f O_*.$(SUF)
		rm -f O_*.c
		rm -f $(NOFILES) main.$(SUF) mkstrct.$(SUF) $(POFILES)
		rm -f $(GENFILES) parser em_nopt $(LIBOPT)

# How to build stand alone version of the optimizer

NOPTLIB = 	$(EMHOME)/modules/lib/libread_emk.$(LIBSUF)\
		$(EMHOME)/modules/lib/libemk.$(LIBSUF)\
		$(EMHOME)/modules/lib/liballoc.$(LIBSUF)\
		$(EMHOME)/modules/lib/malloc.$(SUF)\
		$(EMHOME)/modules/lib/libprint.$(LIBSUF)\
		$(EMHOME)/modules/lib/libstring.$(LIBSUF)\
		$(EMHOME)/modules/lib/libsystem.$(LIBSUF)\
		$(EMHOME)/lib/em_data.$(LIBSUF)

em_nopt:	dfadummy main.$(SUF) $(NOFILES)
		$(CC) -o em_nopt main.$(SUF) $(NOFILES) $(NOPTLIB)

OLINT =		main.c mkstrct.c nopt.c aux.c dfa.c trans.c

OLINTLIB = 	$(EMHOME)/modules/lib/llib-lread_emkV.ln\
		$(EMHOME)/modules/lib/llib-lemk.ln\
		$(EMHOME)/modules/lib/llib-lsystem.ln\
		$(EMHOME)/modules/lib/llib-lalloc.ln\
		$(EMHOME)/modules/lib/llib-lprint.ln\
		$(EMHOME)/modules/lib/llib-lstring.ln

lintnopt:	dfadummy $(OLINT)
		$(LINT) $(LINTFLAGS) $(OLINT) $(OLINTLIB)

# How to build the library version of the optimizer

$(LIBOPT):	dfadummy $(NOFILES) mkstrct.$(SUF) pseudo.d incalls.d
		rm -f $(LIBOPT)
		ar rc $(LIBOPT) O_*.$(SUF) $(NOFILES) mkstrct.$(SUF)
		-sh -c 'ranlib $(LIBOPT)'

$(LIBCEOPT):
		make clean
		make "EMHOME="$(EMHOME) PREFLAGS='$(INCLDIR) -DPRIVATE=static -DCODE_EXPANDER' $(LIBOPT)
		mv $(LIBOPT) $(LIBCEOPT)
		make clean

dfadummy:	patterns parser
		-$(EMHOME)/lib/cpp patterns | $(HOWMUCH) >/tmp/patts
		parser </tmp/patts
		-rm /tmp/patts
		touch dfadummy

# How to build the lint library for the optimizer

LLINT =		nopt.c mkstrct.c aux.c dfa.c trans.c

lintlib:	dfadummy $(LLINT)
		$(LINT) $(LINTFLAGS) $(LLINT) -Cemopt
		mv llib-lemopt.ln $(EMHOME)/modules/lib

# How to build program to parse patterns table and build c files.

PARSERLIB =	$(EMHOME)/lib/em_data.$(LIBSUF)\
		$(EMHOME)/modules/lib/liballoc.$(LIBSUF)\
		$(EMHOME)/modules/lib/libstring.$(LIBSUF)\
		$(EMHOME)/modules/lib/libsystem.$(LIBSUF)

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

main.$(SUF):		nopt.h
nopt.$(SUF):		nopt.h
mkstrct.$(SUF):	nopt.h
aux.$(SUF):		nopt.h
pseudo.d:	nopt.h makefuns.awk
dfa.$(SUF):		nopt.h
trans.$(SUF):	nopt.h
incalls.d:	nopt.h makefuns.awk

parser.$(SUF):	Lpars.h parser.h
Lpars.$(SUF):	Lpars.h
syntax.$(SUF):	syntax.l parser.h Lpars.h
outputdfa.$(SUF):	parser.h Lpars.h
outcalls.$(SUF):	parser.h
findworst.$(SUF):	parser.h
initlex.$(SUF):	parser.h
