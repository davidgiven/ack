EMHOME = ../../..
ETC = $(EMHOME)/etc
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare
CFLAGS = -I$(EMHOME)/h -I$(EMHOME)/modules/h -I. -O $(COPT)
AR = ar
SUF = o
LIBSUF = a
SRC =	bhcst.c bhdlb.c bhdnam.c bhfcon.c bhicon.c bhilb.c bhpnam.c bhucon.c \
	crcst.c crdlb.c crdnam.c crxcon.c crilb.c crpnam.c crscon.c \
	cst.c dfdlb.c dfdnam.c dfilb.c dlb.c dnam.c end.c endarg.c \
	exc.c fcon.c getid.c icon.c ilb.c insert.c internerr.c \
	msend.c op.c opcst.c opdlb.c opdnam.c opilb.c opnarg.c oppnam.c pnam.c \
	pro.c pronarg.c msstart.c psdlb.c psdnam.c pspnam.c scon.c ucon.c \
	C_out.c failed.c em.c

OBS = failed.$(SUF) insert.$(SUF) internerr.$(SUF) getid.$(SUF)

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		em_codeEK.h libeme.$(LIBSUF) libemk.$(LIBSUF) em_code.3

install:	all
		$(INSTALL) lib/libeme.$(LIBSUF)
		$(INSTALL) lib/libemk.$(LIBSUF)
		$(INSTALL) man/em_code.3
		$(INSTALL) h/em_codeEK.h

compare:	all
		-$(COMPARE) lib/libeme.$(LIBSUF)
		-$(COMPARE) lib/libemk.$(LIBSUF)
		-$(COMPARE) man/em_code.3
		-$(COMPARE) h/em_codeEK.h

em_code.3:	em_code.3X
		-sh -c 'tbl < em_code.3X > em_code.3'
		-sh -c 'if test -s em_code.3 ; then : ; else cp em_code.3X em_code.3 ; fi '

libeme.$(LIBSUF):	em_private.h $(SRC) $(OBS)
		EMHOME=$(EMHOME); cc="$(CC)"; suf="$(SUF)"; libsuf="$(LIBSUF)"; cflags="-c -DREADABLE_EM $(CFLAGS)"; ar="$(AR)"; export EMHOME cc ar suf libsuf cflags; sh make.sh e $(SRC)
		-sh -c 'ranlib libeme.$(LIBSUF)'

libemk.$(LIBSUF):	em_private.h $(SRC) $(OBS)
		EMHOME=$(EMHOME); cc="$(CC)"; suf="$(SUF)"; libsuf="$(LIBSUF)"; cflags="-c $(CFLAGS)"; ar="$(AR)"; export EMHOME cc ar suf libsuf cflags; sh make.sh k $(SRC)
		-sh -c 'ranlib libemk.$(LIBSUF)'

em_codeEK.h:	make.em.gen $(ETC)/em_table em.nogen
		make.em.gen $(ETC)/em_table > em_codeEK.h
		cat em.nogen >> em_codeEK.h

pr:
		@pr Makefile em.nogen make.em.gen make.sh insert.h $(SRC) em_private.h

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF) em_code.3 em_codeEK.h

lintlib:	make.sh
		lint -I. -I../../h -I../../../h -Ceme -DREADABLE_EM $(SRC)
		lint -I. -I../../h -I../../../h -Cemk $(SRC)
		mv llib-leme.ln llib-lemk.ln $(EMHOME)/modules/lib

insert.$(SUF):	insert.c insert.h
		$(CC) $(CFLAGS) -c insert.c
