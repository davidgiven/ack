# $Header$

#PARAMS		do not remove this line!

ETC = $(SRC_HOME)/etc
SRC_DIR = $(SRC_HOME)/modules/src/em_code
MOD_DIR = $(TARGET_HOME)/modules

INCLUDES = -I$(TARGET_HOME)/h -I$(TARGET_HOME)/config -I$(MOD_DIR)/h -I$(SRC_HOME)/modules/src/read_em -I$(SRC_DIR)
CFLAGS = $(INCLUDES) $(COPTIONS)
SRC =	$(SRC_DIR)/bhcst.c\
	$(SRC_DIR)/bhdlb.c\
	$(SRC_DIR)/bhdnam.c\
	$(SRC_DIR)/bhfcon.c\
	$(SRC_DIR)/bhicon.c\
	$(SRC_DIR)/bhilb.c\
	$(SRC_DIR)/bhpnam.c\
	$(SRC_DIR)/bhucon.c\
	$(SRC_DIR)/crcst.c\
	$(SRC_DIR)/crdlb.c\
	$(SRC_DIR)/crdnam.c\
	$(SRC_DIR)/crxcon.c\
	$(SRC_DIR)/crilb.c\
	$(SRC_DIR)/crpnam.c\
	$(SRC_DIR)/crscon.c \
	$(SRC_DIR)/cst.c\
	$(SRC_DIR)/dfdlb.c\
	$(SRC_DIR)/dfdnam.c\
	$(SRC_DIR)/dfilb.c\
	$(SRC_DIR)/dlb.c\
	$(SRC_DIR)/dnam.c\
	$(SRC_DIR)/end.c\
	$(SRC_DIR)/endarg.c \
	$(SRC_DIR)/exc.c\
	$(SRC_DIR)/fcon.c\
	$(SRC_DIR)/getid.c\
	$(SRC_DIR)/icon.c\
	$(SRC_DIR)/ilb.c\
	$(SRC_DIR)/insert.c\
	$(SRC_DIR)/internerr.c \
	$(SRC_DIR)/msend.c\
	$(SRC_DIR)/op.c\
	$(SRC_DIR)/opcst.c\
	$(SRC_DIR)/opdlb.c\
	$(SRC_DIR)/opdnam.c\
	$(SRC_DIR)/opilb.c\
	$(SRC_DIR)/opnarg.c\
	$(SRC_DIR)/oppnam.c\
	$(SRC_DIR)/pnam.c \
	$(SRC_DIR)/pro.c\
	$(SRC_DIR)/pronarg.c\
	$(SRC_DIR)/msstart.c\
	$(SRC_DIR)/psdlb.c\
	$(SRC_DIR)/psdnam.c\
	$(SRC_DIR)/pspnam.c\
	$(SRC_DIR)/scon.c\
	$(SRC_DIR)/ucon.c \
	$(SRC_DIR)/C_out.c\
	$(SRC_DIR)/failed.c\
	$(SRC_DIR)/em.c

OBS = failed.$(SUF) insert.$(SUF) internerr.$(SUF) getid.$(SUF)

all:		em_codeEK.h libeme.$(LIBSUF) libemk.$(LIBSUF) em_code.3

install:	all lintlib
		cp libeme.$(LIBSUF) $(MOD_DIR)/lib/libeme.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libeme.$(LIBSUF)
		cp libemk.$(LIBSUF) $(MOD_DIR)/lib/libemk.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libemk.$(LIBSUF)
		cp em_code.3 $(MOD_DIR)/man/em_code.3
		cp em_codeEK.h $(MOD_DIR)/h/em_codeEK.h

compare:	all
		-cmp libeme.$(LIBSUF) $(MOD_DIR)/lib/libeme.$(LIBSUF)
		-cmp libemk.$(LIBSUF) $(MOD_DIR)/lib/libemk.$(LIBSUF)
		-cmp em_code.3 $(MOD_DIR)/man/em_code.3
		-cmp em_codeEK.h $(MOD_DIR)/h/em_codeEK.h

em_code.3:	$(SRC_DIR)/em_code.3X
		-sh -c 'tbl < $(SRC_DIR)/em_code.3X > em_code.3'
		-sh -c 'if test -s em_code.3 ; then : ; else cp $(SRC_DIR)/em_code.3X em_code.3 ; fi '

libeme.$(LIBSUF):	$(SRC_DIR)/em_private.h $(SRC) $(OBS)
		cc="$(CC)"; suf="$(SUF)"; libsuf="$(LIBSUF)"; cflags="-c -DREADABLE_EM $(CFLAGS)"; ar="$(AR)"; export cc ar suf libsuf cflags; sh $(SRC_DIR)/make.sh e $(SRC)
		$(RANLIB) libeme.$(LIBSUF)

libemk.$(LIBSUF):	$(SRC_DIR)/em_private.h $(SRC) $(OBS)
		cc="$(CC)"; suf="$(SUF)"; libsuf="$(LIBSUF)"; cflags="-c $(CFLAGS)"; ar="$(AR)"; export cc ar suf libsuf cflags; sh $(SRC_DIR)/make.sh k $(SRC)
		$(RANLIB) libemk.$(LIBSUF)

em_codeEK.h:	$(SRC_DIR)/make.em.gen $(ETC)/em_table $(SRC_DIR)/em.nogen
		$(SRC_DIR)/make.em.gen $(ETC)/em_table > em_codeEK.h
		cat $(SRC_DIR)/em.nogen >> em_codeEK.h

pr:
		@pr $(SRC_DIR)/proto.make \
			$(SRC_DIR)/em.nogen \
			$(SRC_DIR)/make.em.gen \
			$(SRC_DIR)/make.sh \
			$(SRC_DIR)/insert.h \
			$(SRC) \
			$(SRC_DIR)/em_private.h

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF) em_code.3 em_codeEK.h

lintlib:	$(SRC_DIR)/make.sh
		$(MK_LINT_LIB) eme $(MOD_DIR)/lib $(INCLUDES) -DREADABLE_EM $(SRC)
		$(MK_LINT_LIB) emk $(MOD_DIR)/lib $(INCLUDES) $(SRC)

insert.$(SUF):	$(SRC_DIR)/insert.c $(SRC_DIR)/insert.h
		$(CC) $(CFLAGS) -c $(SRC_DIR)/insert.c

failed.$(SUF):	$(SRC_DIR)/failed.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/failed.c

internerr.$(SUF):	$(SRC_DIR)/internerr.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/internerr.c

getid.$(SUF):	$(SRC_DIR)/getid.c
		$(CC) $(CFLAGS) -c $(SRC_DIR)/getid.c
