# $Header$

#PARAMS		do not remove this line!

CEGLIB = $(TARGET_HOME)/lib.bin/ceg
PARLIB = $(CEGLIB)/EM_parser
LIB = $(PARLIB)/obj_EM_pars

SRC_DIR = $(SRC_HOME)/util/ceg/EM_parser/obj_EM_pars

OFILES = dist.$(SUF)
HFILES = $(SRC_DIR)/arg_type.h $(SRC_DIR)/em_parser.h

IFILES = -I$(TARGET_HOME)/h -I$(TARGET_HOME)/modules/h
CFLAGS = $(COPTIONS) $(IFILES) -DNDEBUG

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
	@pr $(SRC_DIR)/proto.make $(HFILES) $(SRC_DIR)/dist.c

opr:
	make pr | opr

dist.$(SUF):	$(SRC_DIR)/dist.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/dist.c
