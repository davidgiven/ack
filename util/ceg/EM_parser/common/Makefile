EM=/proj/em/Work

GFILES = pars.g
OFILES = pars.o Lpars.o scan.o mylex.o default.o C_instr2.o assem.o eval.o\
	 action.o
IFILES = -I$(EM)/h -I$(EM)/modules/h
CC = cc

.c.o :
	$(CC) $(IFILES) -c $<
	rm -f ../obj_EM_pars/$*.o
	rm -f ../as_EM_pars/$*.o
	ln $*.o ../obj_EM_pars/$*.o
	ln $*.o ../as_EM_pars/$*.o

common : dummy $(OFILES)
	touch ../*/em_parser

dummy : $(GFILES)
	LLgen $(LLOPT) $(GFILES)
	touch dummy

Lpars.o	: Lpars.h decl.h
pars.o	: Lpars.h decl.h
Lpars.o	: Lpars.h decl.h
