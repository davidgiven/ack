EM=../../../..
CEGLIB = $(EM)/lib/ceg
PARLIB = $(CEGLIB)/EM_parser
LIB = $(PARLIB)/as_EM_pars

OFILES = dist.o error.o
HFILES = arg_type.h em_decl.h em_parser.h

IFILES = -I$(EM)/h -I$(EM)/modules/h
CC = cc

.c.o :
	$(CC) $(IFILES) -c $<

all :	$(OFILES)

install:	all
	-mkdir $(CEGLIB)
	-mkdir $(PARLIB)
	-mkdir $(LIB)
	cp $(OFILES) $(HFILES) $(LIB)

cmp:	all
	for i in $(OFILES) $(HFILES) ; do cmp $$i $(LIB)/$$i ; done

clean:
	rm -f *.o

pr:
	@pr Makefile arg_type.h dist.c em_decl.h em_parser.h error.c

opr:
	make pr | opr
