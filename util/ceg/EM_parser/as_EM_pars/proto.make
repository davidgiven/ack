# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ceg/EM_parser/as_EM_pars

CEGLIB = $(TARGET_HOME)/lib.bin/ceg
PARLIB = $(CEGLIB)/EM_parser
LIB = $(PARLIB)/as_EM_pars

OFILES = dist.$(SUF) error.$(SUF)
HFILES = $(SRC_DIR)/arg_type.h $(SRC_DIR)/em_decl.h $(SRC_DIR)/em_parser.h

IFILES = -I$(TARGET_HOME)/h -I$(TARGET_HOME)/modules/h

CFLAGS = $(COPTIONS) $(IFILES)

all :	$(OFILES)

install:	all
	-mkdir $(CEGLIB)
	-mkdir $(PARLIB)
	-mkdir $(LIB)
	cp $(OFILES) $(HFILES) $(LIB)

cmp:	all
	for i in $(OFILES) $(HFILES) ; do cmp $$i $(LIB)/$$i ; done

clean:
	rm -f *.$(SUF)

pr:
	@pr $(SRC_DIR)/proto.make $(HFILES) $(SRC_DIR)/dist.c $(SRC_DIR)/error.c

opr:
	make pr | opr

dist.$(SUF):	$(SRC_DIR)/dist.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/dist.c

error.$(SUF):	$(SRC_DIR)/error.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/error.c
