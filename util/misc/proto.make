# $Header$

#PARAMS		do not remove this line!

h=$(TARGET_HOME)/h
l=$(TARGET_HOME)/lib.bin
ml=$(TARGET_HOME)/modules/lib
mh=$(TARGET_HOME)/modules/h
ul=$(UTIL_HOME)/lib.bin
uml=$(UTIL_HOME)/modules/lib

SRC_DIR=$(SRC_HOME)/util/misc

DEC_PATH=decode
ENC_PATH=encode
DATA_PATH=$l/em_data.$(LIBSUF)
DECLIBS=$(ml)/libread_emkV.$(LIBSUF) $(ml)/libeme.$(LIBSUF)\
	$(ml)/liballoc.$(LIBSUF) $(ml)/libprint.$(LIBSUF)\
	$(ml)/libstring.$(LIBSUF) $(ml)/libsystem.$(LIBSUF)
ENCLIBS=$(ml)/libread_emeV.$(LIBSUF) $(ml)/libemk.$(LIBSUF) \
	$(ml)/liballoc.$(LIBSUF) $(ml)/libprint.$(LIBSUF) \
	$(ml)/libstring.$(LIBSUF) $(ml)/libsystem.$(LIBSUF)
LINTLIBS= $(ul)/$(LINTPREF)em_data.$(LINTSUF) \
	$(uml)/$(LINTPREF)read_emkV.$(LINTSUF) \
	$(uml)/$(LINTPREF)eme.$(LINTSUF) \
	$(uml)/$(LINTPREF)alloc.$(LINTSUF) \
	$(uml)/$(LINTPREF)print.$(LINTSUF) \
	$(uml)/$(LINTPREF)string.$(LINTSUF) \
	$(uml)/$(LINTPREF)system.$(LINTSUF)
HFILES=$h/em_mnem.h $h/em_spec.h $h/em_pseu.h $h/em_flag.h $h/em_ptyp.h  \
	$h/em_mes.h $(mh)/em.h $(mh)/em_comp.h

CFLAGS=$(COPTIONS) -I$(mh) -I$h
LDFLAGS = $(LDOPTIONS)
LINTFLAGS = $(LINTOPTIONS) -I$(mh) -I$h

all:            $(DEC_PATH) $(ENC_PATH) esize

$(DEC_PATH):    decode.$(SUF)
		$(CC) $(LDFLAGS) -o $(DEC_PATH) decode.$(SUF) $(DECLIBS) $(DATA_PATH)

$(ENC_PATH):    encode.$(SUF)
		$(CC) $(LDFLAGS) -o $(ENC_PATH) encode.$(SUF) $(ENCLIBS) $(DATA_PATH)

esize:		esize.$(SUF)
		$(CC) $(LDFLAGS) -o esize esize.$(SUF)

convert.$(SUF):	$(HFILES) $(SRC_DIR)/convert.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/convert.c

esize.$(SUF):	$(SRC_DIR)/esize.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/esize.c

encode.$(SUF):	convert.$(SUF)
		cp convert.$(SUF) encode.$(SUF)

decode.$(SUF):	convert.$(SUF)
		cp convert.$(SUF) decode.$(SUF)

clean:
		rm -f $(DEC_PATH) $(ENC_PATH) esize *.$(SUF) *.old

install :       all
		cp $(DEC_PATH) $l/em_$(DEC_PATH)
		cp $(ENC_PATH) $l/em_$(ENC_PATH)
		cp esize $(TARGET_HOME)/bin/esize
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/em_decode.6 $(TARGET_HOME) ; \
			mk_manpage $(SRC_DIR)/esize.1 $(TARGET_HOME) ; \
		fi

cmp :           all
		-cmp $(DEC_PATH) $l/em_$(DEC_PATH)
		-cmp $(ENC_PATH) $l/em_$(ENC_PATH)
		-cmp esize $(TARGET_HOME)/bin/esize

opr:
		make pr ^ opr
pr:
		@pr -n $(SRC_DIR)/proto.make $(SRC_DIR)/convert.c $(SRC_DIR)/esize.c

lint:
		$(LINT) $(LINTFLAGS) $(SRC_DIR)/esize.c
		$(LINT) $(LINTFLAGS) $(SRC_DIR)/convert.c $(LINTLIBS)
