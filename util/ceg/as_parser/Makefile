EM=/proj/em/Work

GFILES = pars.g
OFILES = pars.o Lpars.o lex.yy.o help.o conversion.o
IFILES = -I$(EM)/h -I$(EM)/modules/h
LIBS   = $(EM)/modules/lib/liballoc.a\
	 $(EM)/modules/lib/libprint.a\
	 $(EM)/modules/lib/libstring.a\
	 $(EM)/modules/lib/libsystem.a
CC = cc

.c.o :
	$(CC) $(IFILES) -c $<

as_parser : dummy $(OFILES) 
	  $(CC) -o as_parser $(OFILES) $(LIBS) -ll

lex.yy.c : table.l
	lex table.l

dummy : $(GFILES)
	LLgen $(LLOPT) $(GFILES)
	touch dummy

Lpars.o	: Lpars.h decl.h
pars.o	: Lpars.h decl.h
Lpars.o	: Lpars.h decl.h
lex.yy.o : Lpars.h decl.h
