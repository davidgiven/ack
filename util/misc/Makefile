# $Header$

EMHOME=../..
h=$(EMHOME)/h
l=$(EMHOME)/lib
ml=$(EMHOME)/modules/lib
mh=$(EMHOME)/modules/h

DEC_PATH=decode
ENC_PATH=encode
DATA_PATH=$l/em_data.a
DECLIBS=$(ml)/libread_emkV.a $(ml)/libeme.a $(ml)/liballoc.a $(ml)/libprint.a $(ml)/libstring.a \
	$(ml)/libsystem.a
ENCLIBS=$(ml)/libread_emeV.a $(ml)/libemk.a $(ml)/liballoc.a $(ml)/libprint.a $(ml)/libstring.a \
	$(ml)/libsystem.a
HFILES=$h/em_mnem.h $h/em_spec.h $h/em_pseu.h $h/em_flag.h $h/em_ptyp.h  \
	$h/em_mes.h $(mh)/em.h $(mh)/em_comp.h

CFLAGS=-O -I$(mh) -I$h
LDFLAGS = 

all:            $(DEC_PATH) $(ENC_PATH) esize

$(DEC_PATH):    decode.o $(DATA_PATH)
		$(CC) $(LDFLAGS) -o $(DEC_PATH) decode.o $(DECLIBS) $(DATA_PATH)

$(ENC_PATH):    encode.o $(DATA_PATH)
		$(CC) $(LDFLAGS) -o $(ENC_PATH) encode.o $(ENCLIBS) $(DATA_PATH)

esize:		esize.o
		$(CC) -o esize esize.o

convert.o:	$(HFILES)

encode.o:	convert.o
		cp convert.o encode.o

decode.o:	convert.o
		cp convert.o decode.o

clean:
		rm -f $(DEC_PATH) $(ENC_PATH) esize *.o *.old

install :       all
		rm -f $l/em_$(DEC_PATH) $l/em_$(ENC_PATH) $(EMHOME)/bin/esize $(EMHOME)/man/em_decode.6 $(EMHOME)/man/esize.1
		cp $(DEC_PATH) $l/em_$(DEC_PATH)
		cp $(ENC_PATH) $l/em_$(ENC_PATH)
		cp esize $(EMHOME)/bin/esize
		cp em_decode.6 $(EMHOME)/man/em_decode.6
		cp esize.1 $(EMHOME)/man/esize.1

cmp :           all
		-cmp $(DEC_PATH) $l/em_$(DEC_PATH)
		-cmp $(ENC_PATH) $l/em_$(ENC_PATH)
		-cmp esize $(EMHOME)/bin/esize
		-cmp em_decode.6 $(EMHOME)/man/em_decode.6
		-cmp esize.1 $(EMHOME)/man/esize.1

opr:
		make pr ^ opr
pr:
		@pr -n Makefile convert.c esize.c
