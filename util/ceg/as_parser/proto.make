# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ceg/as_parser
CEGLIB = $(TARGET_HOME)/lib.bin/ceg

IFILES = -I$(EM)/h -I$(EM)/modules/h -I.
CFLAGS = $(COPTIONS) -DFLEX $(IFILES)
LDFLAGS = $(LDOPTIONS)

GFILES = pars.g
OFILES = pars.$(SUF) Lpars.$(SUF) lex.yy.$(SUF) help.$(SUF) conversion.$(SUF)
LIBS   = $(TARGET_HOME)/modules/lib/liballoc.$(LIBSUF)\
	 $(TARGET_HOME)/modules/lib/libprint.$(LIBSUF)\
	 $(TARGET_HOME)/modules/lib/libstring.$(LIBSUF)\
	 $(TARGET_HOME)/modules/lib/libsystem.$(LIBSUF)

all:	as_parser eval/eval

as_parser:	dummy
	make $(OFILES)
	$(CC) $(LDFLAGS) -o as_parser $(OFILES) $(LIBS)

eval/eval:
	( cd eval ; make eval )

install:	all
	-mkdir $(CEGLIB)
	-mkdir $(CEGLIB)/as_parser
	-mkdir $(CEGLIB)/as_parser/eval
	cp as_parser $(CEGLIB)/as_parser/as_parser
	cp eval/eval $(CEGLIB)/as_parser/eval/eval
	cp $(SRC_DIR)/as_parser.h $(CEGLIB)/as_parser/as_parser.h

cmp:	all
	-cmp as_parser $(CEGLIB)/as_parser/as_parser
	-cmp eval/eval $(CEGLIB)/as_parser/eval/eval
	-cmp $(SRC_DIR)/as_parser.h $(CEGLIB)/as_parser/as_parser.h

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/as_parser.h $(SRC_DIR)/const.h \
	$(SRC_DIR)/conversion.c $(SRC_DIR)/decl.h $(SRC_DIR)/help.c \
	$(SRC_DIR)/pars.g $(SRC_DIR)/table.l $(SRC_DIR)/eval/proto.make \
	$(SRC_DIR)/eval/eval.c $(SRC_DIR)/eval/states.h

opr:
	make pr | opr

clean:
	rm -f as_parser *.$(SUF) lex.yy.c Lpars.[ch] pars.c dummy eval/*.$(SUF) eval/eval

lex.yy.c : table.l
	flex -s table.l

dummy : $(GFILES)
	LLgen $(LLOPT) $(GFILES)
	touch dummy

Lpars.$(SUF):	Lpars.h $(SRC_DIR)/decl.h Lpars.c
		$(CC) $(CFLAGS) -c Lpars.c

pars.$(SUF):	Lpars.h $(SRC_DIR)/decl.h pars.c
		$(CC) $(CFLAGS) -c pars.c

lex.yy.$(SUF):	Lpars.h $(SRC_DIR)/decl.h lex.yy.c
		$(CC) $(CFLAGS) -c lex.yy.c

help.$(SUF):	$(SRC_DIR)/help.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/help.c

conversion.$(SUF):	$(SRC_DIR)/conversion.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/conversion.c
