# $Header$

d=../..
h=$d/h
l=$d/lib
ml=$d/modules/lib
mh=$d/modules/h

DEC_PATH=decode
ENC_PATH=encode
DATA_PATH=$l/em_data.a
DECLIBS=$(ml)/libread_emk.a $(ml)/libeme.a $(ml)/libprint.a $(ml)/libstring.a \
	$(ml)/libsystem.a
ENCLIBS=$(ml)/libread_emeV.a $(ml)/libemk.a $(ml)/libprint.a $(ml)/libstring.a \
	$(ml)/libsystem.a
HFILES=$h/em_mnem.h $h/em_spec.h $h/em_pseu.h $h/em_flag.h $h/em_ptyp.h  \
	$h/em_mes.h $(mh)/em.h $(mh)/em_comp.h

CFLAGS=-O -I$(mh) -I$h
LDFLAGS = -i

all:            $(DEC_PATH) $(ENC_PATH)

$(DEC_PATH):    decode.o $(DATA_PATH)
		cc $(LDFLAGS) -o $(DEC_PATH) decode.o $(DECLIBS) $(DATA_PATH)

$(ENC_PATH):    encode.o $(DATA_PATH)
		cc $(LDFLAGS) -o $(ENC_PATH) encode.o $(ENCLIBS) $(DATA_PATH)

convert.o:	$(HFILES)

encode.o:	convert.o
		cp convert.o encode.o

decode.o:	convert.o
		cp convert.o decode.o

clean:
		rm -f $(DEC_PATH) $(ENC_PATH) *.o *.old

install :       all
		rm -f $l/em_$(DEC_PATH) $l/em_$(ENC_PATH) $d/man/em_decode.6
		cp $(DEC_PATH) $l/em_$(DEC_PATH)
		cp $(ENC_PATH) $l/em_$(ENC_PATH)
		cp em_decode.6 $d/man/em_decode.6

cmp :           all
		-cmp $(DEC_PATH) $l/em_$(DEC_PATH)
		-cmp $(ENC_PATH) $l/em_$(ENC_PATH)
		-cmp em_decode.6 $d/man/em_decode.6

opr:
		make pr ^ opr
pr:
		@pr -n Makefile convert.c
