EM=../../../..
LIB=$(EM)/lib/ceg/EM_parser/as_EM_pars

OFILES = dist.o error.o

IFILES = -I$(EM)/h -I$(EM)/modules/h
CC = cc

.c.o :
	$(CC) $(IFILES) -c $<

all :	$(OFILES)

install:	all
	cp $(OFILES) $(LIB)

cmp:	all
	for i in $(OFILES) ; do cmp $$i $(LIB)/$$i ; done

clean:
	rm -f *.o

opr:

pr:
