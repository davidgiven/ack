# $Header$

#PARAMS		do not remove this line!

CEGLIB = $(TARGET_HOME)/lib.bin/ceg
PARLIB = $(CEGLIB)/EM_parser
OBJLIB = $(PARLIB)/obj_EM_pars
ASLIB = $(PARLIB)/as_EM_pars

GFILES = $(SRC_DIR)/pars.g
OFILES = pars.$(SUF) Lpars.$(SUF) scan.$(SUF) mylex.$(SUF) default.$(SUF) \
	 C_instr2.$(SUF) help.$(SUF) eval.$(SUF) action.$(SUF)

IFILES = -I$(TARGET_HOME)/h -I$(TARGET_HOME)/modules/h -I.
CFLAGS = $(COPTIONS) $(IFILES)

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
	rm -f *.$(SUF) dummy Lpars.[ch] pars.c

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/C_instr2.c $(SRC_DIR)/action.c \
		$(SRC_DIR)/arg_type.h $(SRC_DIR)/decl.h $(SRC_DIR)/default.c \
		$(SRC_DIR)/em_parser.h $(SRC_DIR)/eval.c $(SRC_DIR)/help.c \
		$(SRC_DIR)/mylex.c $(SRC_DIR)/pars.g $(SRC_DIR)/scan.c

opr:
	make pr | opr

Lpars.$(SUF):	Lpars.h $(SRC_DIR)/decl.h Lpars.c
		$(CC) $(CFLAGS) -c Lpars.c

pars.$(SUF):	Lpars.h $(SRC_DIR)/decl.h pars.c
		$(CC) $(CFLAGS) -c pars.c

scan.$(SUF):	$(SRC_DIR)/scan.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/scan.c

mylex.$(SUF):	$(SRC_DIR)/mylex.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/mylex.c

default.$(SUF):	$(SRC_DIR)/default.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/default.c

C_instr2.$(SUF):	$(SRC_DIR)/C_instr2.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/C_instr2.c

help.$(SUF):	$(SRC_DIR)/help.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/help.c

eval.$(SUF):	$(SRC_DIR)/eval.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/eval.c

action.$(SUF):	$(SRC_DIR)/action.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/action.c
