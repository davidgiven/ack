# $Header$
d=../..
h=$d/h
l=$d/lib
AR=ar
SUF=o
LIBSUF=a

OBJ=em_mnem.$(SUF) em_pseu.$(SUF) em_flag.$(SUF) em_ptyp.$(SUF)

DATA_PATH=em_data.$(LIBSUF)

CFLAGS=-O -I$h $(COPT)

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

$(DATA_PATH):   $(OBJ)
		$(AR) rv $(DATA_PATH) $(OBJ)
		-sh -c 'ranlib $(DATA_PATH)'

em_flag.$(SUF):      $h/em_flag.h

em_ptyp.$(SUF):      $h/em_flag.h $h/em_ptyp.h

install :       $(DATA_PATH)
		-cmp -s $(DATA_PATH) $l/$(DATA_PATH) || { cp $(DATA_PATH) $l/$(DATA_PATH) ; sh -c 'ranlib $l/$(DATA_PATH)' ; }

cmp :           $(DATA_PATH)
		-cmp $(DATA_PATH) $l/$(DATA_PATH)

clean:
		rm -f $(OBJ) $(DATA_PATH) *.old

opr:
		make pr ^ opr

pr:
		@pr Makefile em_mnem.c em_pseu.c em_flag.c em_ptyp.c
