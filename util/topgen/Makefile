EM = ../..
CFLAGS = -O
SOURCE = token.h symtab.h misc.h tunable.h main.c topgen.g LLlex.c symtab.c pattern.c hash.c
CFILES = main.c topgen.c Lpars.c LLlex.c symtab.c pattern.c hash.c
OFILES = main.o topgen.o Lpars.o LLlex.o symtab.o pattern.o hash.o

all:		parser
		@make topgen

cmp:		all
		cmp topgen $(EM)/lib/topgen

install:	all
		cp topgen $(EM)/lib/topgen

clean:
		rm -f topgen *.o Lpars.c Lpars.h topgen.c parser

parser:		topgen.g
		$(EM)/bin/LLgen topgen.g
		touch parser

topgen.o:	token.h Lpars.h symtab.h misc.h
Lpars.o:	Lpars.h
LLlex.o:	token.h Lpars.h tunable.h
symtab.o:	symtab.h
hash.o:		misc.h
pattern.o:	misc.h symtab.h

topgen:		$(OFILES)
		$(CC) $(OFILES) -o topgen

lint:		parser
		lint $(CFILES)

pr:
		@pr $(SOURCE) Makefile

opr:
		make pr ^ opr
