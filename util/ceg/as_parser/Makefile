EM=../../..
CEGLIB = $(EM)/lib/ceg

GFILES = pars.g
OFILES = pars.o Lpars.o lex.yy.o help.o conversion.o
IFILES = -I$(EM)/h -I$(EM)/modules/h
LIBS   = $(EM)/modules/lib/liballoc.a\
	 $(EM)/modules/lib/libprint.a\
	 $(EM)/modules/lib/libstring.a\
	 $(EM)/modules/lib/libsystem.a
CC = cc
CFLAGS = -O
LEXLIB = -ll

.c.o :
	$(CC) $(CFLAGS) $(IFILES) -c $<

all:	as_parser eval/eval

as_parser:	dummy
	make $(OFILES)
	$(CC) -o as_parser $(OFILES) $(LIBS) $(LEXLIB)

eval/eval:
	( cd eval ; make eval )

install:	all
	-mkdir $(CEGLIB)
	-mkdir $(CEGLIB)/as_parser
	-mkdir $(CEGLIB)/as_parser/eval
	cp as_parser $(CEGLIB)/as_parser/as_parser
	cp eval/eval $(CEGLIB)/as_parser/eval/eval
	cp as_parser.h $(CEGLIB)/as_parser/as_parser.h

cmp:	all
	-cmp as_parser $(CEGLIB)/as_parser/as_parser
	-cmp eval/eval $(CEGLIB)/as_parser/eval/eval
	-cmp as_parser.h $(CEGLIB)/as_parser/as_parser.h

pr:
	@pr Makefile as_parser.h const.h conversion.c decl.h help.c pars.g table.l eval/Makefile eval/eval.c eval/states.h

opr:
	make pr | opr

clean:
	rm -f as_parser *.o lex.yy.c Lpars.[ch] pars.c dummy eval/*.o eval/eval

lex.yy.c : table.l
	lex table.l

dummy : $(GFILES)
	LLgen $(LLOPT) $(GFILES)
	touch dummy

Lpars.o	: Lpars.h decl.h
pars.o	: Lpars.h decl.h
Lpars.o	: Lpars.h decl.h
lex.yy.o : Lpars.h decl.h
