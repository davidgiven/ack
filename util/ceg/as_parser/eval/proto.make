# $Id$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ceg/as_parser/eval

CFLAGS = $(COPTIONS) -DNDEBUG
LDFLAGS = $(LDOPTIONS)

eval:		eval.$(SUF)
		$(CC) $(LDFLAGS) -o eval eval.$(SUF)

eval.$(SUF):	$(SRC_DIR)/states.h $(SRC_DIR)/eval.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/eval.c
