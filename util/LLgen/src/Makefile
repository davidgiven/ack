# $Id$
PROF=
LLOPT= # -vvv -x
INCLUDES =
DEFINES = -DNDEBUG -DNON_CORRECTING
CFLAGS=-O $(DEFINES) $(INCLUDES) $(PROF)
LDFLAGS=
OBJECTS = main.o gencode.o compute.o LLgen.o tokens.o check.o reach.o global.o name.o sets.o Lpars.o alloc.o machdep.o cclass.o savegram.o
CFILES = main.c gencode.c compute.c LLgen.c tokens.c check.c reach.c global.c name.c sets.c Lpars.c alloc.c machdep.c cclass.c savegram.c
FILES =types.h extern.h io.h sets.h tokens.g LLgen.g main.c name.c compute.c sets.c gencode.c global.c check.c reach.c alloc.c machdep.c Makefile cclass.c savegram.c
GFILES = tokens.g LLgen.g
LINT = lint -b $(DEFINES) -DNORCSID

LIBDIRSTR = \"$(LIBDIR)\"

all:		parser
		@make LLgen "LDFLAGS=$(LDFLAGS)" "CC=$(CC)" "CFLAGS=$(CFLAGS)" "PROF=$(PROF)" "LIBDIR=$(LIBDIR)"

parser:		$(GFILES)
		LLgen $(LLOPT) $(GFILES)
		@touch parser

first:		firstparser
		@make LLgen "LDFLAGS=$(LDFLAGS)" "CC=$(CC)" "CFLAGS=$(CFLAGS)" "PROF=$(PROF)" "LIBDIR=$(LIBDIR)"

firstparser:
		cp LLgen.c.dist LLgen.c
		cp tokens.c.dist tokens.c
		cp Lpars.c.dist Lpars.c
		cp Lpars.h.dist Lpars.h
		@touch parser

LLgen:		$(OBJECTS)
		$(CC) $(PROF) $(LDFLAGS) $(OBJECTS) -o LLgen
		@size LLgen

pr : 
		@pr $(FILES) ../lib/rec ../lib/incl Makefile

lint: 
		$(LINT) $(INCLUDES) -DLIBDIR=$(LIBDIRSTR) $(CFILES)

clean:
		-rm -f *.o LL.temp LL.xxx LL.output LLgen LLgen.c tokens.c Lpars.[ch] parser

distr:
		-rm -f parser
		make parser
		cp Lpars.c Lpars.c.dist
		cp Lpars.h Lpars.h.dist
		cp LLgen.c LLgen.c.dist
		cp tokens.c tokens.c.dist

machdep.o:	machdep.c
		$(CC) -c $(CFLAGS) -DLIBDIR=$(LIBDIRSTR) machdep.c

# The next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
LLgen.o:	Lpars.h
LLgen.o:	cclass.h
LLgen.o:	extern.h
LLgen.o:	io.h
LLgen.o:	types.h
Lpars.o:	Lpars.h
alloc.o:	extern.h
alloc.o:	types.h
cclass.o:	cclass.h
check.o:	extern.h
check.o:	io.h
check.o:	sets.h
check.o:	types.h
compute.o:	extern.h
compute.o:	io.h
compute.o:	sets.h
compute.o:	types.h
gencode.o:	cclass.h
gencode.o:	extern.h
gencode.o:	io.h
gencode.o:	sets.h
gencode.o:	types.h
global.o:	extern.h
global.o:	io.h
global.o:	types.h
machdep.o:	types.h
main.o:	extern.h
main.o:	io.h
main.o:	sets.h
main.o:	types.h
name.o:	extern.h
name.o:	io.h
name.o:	types.h
reach.o:	extern.h
reach.o:	io.h
reach.o:	types.h
sets.o:	extern.h
sets.o:	sets.h
sets.o:	types.h
tokens.o:	Lpars.h
tokens.o:	cclass.h
tokens.o:	extern.h
tokens.o:	io.h
tokens.o:	types.h
