# $Header$

#PARAMS		do not remove this line!

SRC_DIR =	$(SRC_HOME)/util/topgen
INCLUDES =	-I$(SRC_DIR) -I.
CFLAGS =	$(COPTIONS) $(INCLUDES)
LDFLAGS =	$(LDOPTIONS)
LINTFLAGS =	$(LINTOPTIONS) $(INCLUDES)
SOURCE =	$(SRC_DIR)/token.h $(SRC_DIR)/symtab.h $(SRC_DIR)/misc.h \
		$(SRC_DIR)/tunable.h $(SRC_DIR)/main.c $(SRC_DIR)/topgen.g \
		$(SRC_DIR)/LLlex.c $(SRC_DIR)/symtab.c $(SRC_DIR)/pattern.c \
		$(SRC_DIR)/hash.c
CFILES =	$(SRC_DIR)/main.c topgen.c Lpars.c $(SRC_DIR)/LLlex.c \
		$(SRC_DIR)/symtab.c $(SRC_DIR)/pattern.c $(SRC_DIR)/hash.c
OFILES =	main.$(SUF) topgen.$(SUF) Lpars.$(SUF) LLlex.$(SUF) \
		symtab.$(SUF) pattern.$(SUF) hash.$(SUF)

all:		parser
		@make topgen

cmp:		all
		cmp topgen $(TARGET_HOME)/lib.bin/topgen

install:	all
		cp topgen $(TARGET_HOME)/lib.bin/topgen

clean:
		rm -f topgen *.$(SUF) Lpars.c Lpars.h topgen.c parser

parser:		$(SRC_DIR)/topgen.g
		$(UTIL_HOME)/bin/LLgen $(SRC_DIR)/topgen.g
		touch parser

topgen.$(SUF):	$(SRC_DIR)/token.h Lpars.h $(SRC_DIR)/symtab.h $(SRC_DIR)/misc.h topgen.c
		$(CC) -c $(CFLAGS) topgen.c
Lpars.$(SUF):	Lpars.h Lpars.c
		$(CC) -c $(CFLAGS) Lpars.c
LLlex.$(SUF):	$(SRC_DIR)/token.h Lpars.h $(SRC_DIR)/tunable.h $(SRC_DIR)/LLlex.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/LLlex.c
symtab.$(SUF):	$(SRC_DIR)/symtab.h $(SRC_DIR)/symtab.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/symtab.c
hash.$(SUF):	$(SRC_DIR)/misc.h $(SRC_DIR)/hash.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/hash.c
pattern.$(SUF):	$(SRC_DIR)/misc.h $(SRC_DIR)/symtab.h $(SRC_DIR)/pattern.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/pattern.c
main.$(SUF):	$(SRC_DIR)/main.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/main.c

topgen:		$(OFILES)
		$(CC) $(LDFLAGS) $(OFILES) -o topgen

lint:		parser
		$(LINT) $(LINTFLAGS) $(CFILES)

pr:
		@pr $(SOURCE) $(SRC_DIR)/proto.make

opr:
		make pr ^ opr
