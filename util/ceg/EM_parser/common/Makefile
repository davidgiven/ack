EM=../../../..
CEGLIB = $(EM)/lib/ceg
PARLIB = $(CEGLIB)/EM_parser
OBJLIB = $(PARLIB)/obj_EM_pars
ASLIB = $(PARLIB)/as_EM_pars

GFILES = pars.g
OFILES = pars.o Lpars.o scan.o mylex.o default.o C_instr2.o help.o eval.o\
	 action.o
IFILES = -I$(EM)/h -I$(EM)/modules/h
CC = cc
CFLAGS = $(IFILES)

all :	dummy
	make $(OFILES)

dummy : $(GFILES)
	LLgen $(LLOPT) $(GFILES)
	touch dummy

install:	all
	-mkdir $(CEGLIB)
	-mkdir $(PARLIB)
	-mkdir $(OBJLIB)
	-mkdir $(ASLIB)
	cp $(OFILES) $(ASLIB)
	for i in $(OFILES) ; do rm -f $(OBJLIB)/$$i ; done
	for i in $(OFILES) ; do ln $(ASLIB)/$$i $(OBJLIB)/$$i ; done

cmp:	all
	for i in $(OFILES) ; do cmp $$i $(ASLIB)/$$i ; done

clean:
	rm -f *.o dummy Lpars.[ch] pars.c

pr:
	@pr Makefile C_instr2.c action.c arg_type.h decl.h default.c \
		em_parser.h eval.c help.c mylex.c pars.g scan.c

opr:
	make pr | opr

Lpars.o	: Lpars.h decl.h
pars.o	: Lpars.h decl.h
