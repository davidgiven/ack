# $Header$
d=../..
h=$d/h
l=$d/lib

OBJ=em_mnem.o em_pseu.o em_flag.o em_ptyp.o

DATA_PATH=em_data.a

CFLAGS=-O -I$h

$(DATA_PATH):   $(OBJ)
		ar rv $(DATA_PATH) $(OBJ)
		-sh -c 'ranlib $(DATA_PATH)'

em_flag.o:      $h/em_flag.h

em_ptyp.o:      $h/em_flag.h $h/em_ptyp.h

install :       $(DATA_PATH)
		-cmp -s $(DATA_PATH) $l/$(DATA_PATH) || { cp $(DATA_PATH) $l/$(DATA_PATH) ; sh -c 'ranlib $l/$(DATA_PATH)' ; }

cmp :           $(DATA_PATH)
		cmp $(DATA_PATH) $l/$(DATA_PATH)

clean:
		rm -f $(OBJ) $(DATA_PATH) *.old

opr:
		make pr ^ opr

pr:
		@pr Makefile em_mnem.c em_pseu.c em_flag.c em_ptyp.c
