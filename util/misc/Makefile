# $Header$

d=../..
h=$d/h
l=$d/lib

DEC_PATH=decode
ENC_PATH=encode
DATA_PATH=$l/em_data.a

CFLAGS=-O -I$h

all:            $(DEC_PATH) $(ENC_PATH)

$(DEC_PATH):    decode.o $(DATA_PATH)
		cc -n -o $(DEC_PATH) decode.o $(DATA_PATH)

$(ENC_PATH):    encode.o $(DATA_PATH)
		cc -n -o $(ENC_PATH) encode.o $(DATA_PATH)

encode.o:       $h/em_spec.h $h/em_pseu.h $h/em_flag.h $h/em_ptyp.h $h/em_mes.h

decode.o:       $h/em_spec.h $h/em_pseu.h $h/em_flag.h $h/em_ptyp.h $h/em_mes.h

clean:
		rm -f $(DEC_PATH) $(ENC_PATH) *.o *.old
install :       all
		cp $(DEC_PATH) $l/em_$(DEC_PATH)
		cp $(ENC_PATH) $l/em_$(ENC_PATH)

cmp :           all
		cmp $(DEC_PATH) $l/$(DEC_PATH)
		cmp $(ENC_PATH) $l/$(ENC_PATH)

opr:
		make pr ^ opr
pr:
		@pr -n Makefile decode.c encode.c
