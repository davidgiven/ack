PROF=
LLOPT=-vvv -x
CFLAGS=$(PROF) -O -DNDEBUG # -R
LDFLAGS=-i
OBJECTS = main.o gencode.o compute.o LLgen.o tokens.o check.o reach.o global.o name.o sets.o Lpars.o alloc.o machdep.o
CFILES = main.c gencode.c compute.c LLgen.c tokens.c check.c reach.c global.c name.c sets.c Lpars.c alloc.c machdep.c
FILES =types.h tunable.h extern.h io.h sets.h assert.h tokens.g LLgen.g main.c name.c compute.c sets.c gencode.c global.c check.c reach.c alloc.c machdep.c Makefile
GFILES = tokens.g LLgen.g
LINT = lint -b -DNDEBUG

all:
		@make parser "LLOPT=$(LLOPT)"
		@make LLgen "LDFLAGS=$(LDFLAGS)" "CC=$(CC)" "PROF=$(PROF)" "CFLAGS=$(CFLAGS)"

parser:		$(GFILES)
		LLgen $(LLOPT) $(GFILES)
		@touch parser

LLgen:		$(OBJECTS)
		$(CC) $(PROF) $(LDFLAGS) $(OBJECTS) -o LLgen
		@size LLgen

pr : 
		@pr $(FILES) ../lib/rec ../lib/incl

lint: 
		$(LINT) $(CFILES)

clean:
		-rm -f *.o LL.temp LL.xxx LL.output LLgen

depend:
		./makedepend `grep -v '\.h' sources`

# The next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
LLgen.o:	Lpars.h
LLgen.o:	assert.h
LLgen.o:	extern.h
LLgen.o:	io.h
LLgen.o:	tunable.h
LLgen.o:	types.h
Lpars.o:	Lpars.h
alloc.o:	extern.h
alloc.o:	types.h
check.o:	assert.h
check.o:	extern.h
check.o:	io.h
check.o:	sets.h
check.o:	tunable.h
check.o:	types.h
compute.o:	assert.h
compute.o:	extern.h
compute.o:	io.h
compute.o:	sets.h
compute.o:	tunable.h
compute.o:	types.h
gencode.o:	assert.h
gencode.o:	extern.h
gencode.o:	io.h
gencode.o:	sets.h
gencode.o:	tunable.h
gencode.o:	types.h
global.o:	io.h
global.o:	tunable.h
global.o:	types.h
machdep.o:	types.h
main.o:	assert.h
main.o:	extern.h
main.o:	io.h
main.o:	sets.h
main.o:	types.h
name.o:	assert.h
name.o:	extern.h
name.o:	io.h
name.o:	tunable.h
name.o:	types.h
reach.o:	assert.h
reach.o:	extern.h
reach.o:	io.h
reach.o:	tunable.h
reach.o:	types.h
sets.o:	assert.h
sets.o:	extern.h
sets.o:	sets.h
sets.o:	types.h
tokens.o:	Lpars.h
tokens.o:	assert.h
tokens.o:	extern.h
tokens.o:	io.h
tokens.o:	tunable.h
tokens.o:	types.h
