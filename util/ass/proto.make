# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ass
INCLUDES = -I$(TARGET_HOME)/h -I$(TARGET_HOME)/config -I$(SRC_DIR)
CFLAGS= $(INCLUDES) $(COPTIONS)
UCFLAGS= $(INCLUDES) $(UCOPTIONS)
LDFLAGS = $(LDOPTIONS)
ULDFLAGS = $(ULDOPTIONS)
LINTFLAGS = $(INCLUDES) $(LINTOPTIONS)

OBS =	ass00.$(SUF) ass30.$(SUF) ass40.$(SUF) ass50.$(SUF) ass60.$(SUF) \
	ass70.$(SUF) ass80.$(SUF) assci.$(SUF) assda.$(SUF) assrl.$(SUF) \
	asstb.$(SUF) asscm.$(SUF)
CFILES = $(SRC_DIR)/ass00.c $(SRC_DIR)/ass30.c $(SRC_DIR)/ass40.c \
	$(SRC_DIR)/ass50.c $(SRC_DIR)/ass60.c $(SRC_DIR)/ass70.c \
	$(SRC_DIR)/ass80.c $(SRC_DIR)/assci.c $(SRC_DIR)/assda.c \
	$(SRC_DIR)/assrl.c asstb.c $(SRC_DIR)/asscm.c

all:            ass

clean:
		-rm -f ass *.$(SUF) maktab *.old asstb.c

install :       all
		rm -f $(TARGET_HOME)/lib.bin/em_ass
		cp ass $(TARGET_HOME)/lib.bin/em_ass
		rm -f $(TARGET_HOME)/man/em_ass.6
		cp $(SRC_DIR)/em_ass.6 $(TARGET_HOME)/man/em_ass.6

cmp :           all
		-cmp ass $(TARGET_HOME)/lib.bin/em_ass
		-cmp $(SRC_DIR)/em_ass.6 $(TARGET_HOME)/man/em_ass.6

lint:		asstb.c
		$(LINT) $(LINTFLAGS) $(CFILES)

ass:		$(OBS)
		$(CC) $(LDFLAGS) -o ass $(OBS) $(TARGET_HOME)/lib.bin/em_data.$(LIBSUF)

asstb.c:        maktab $(SRC_HOME)/etc/ip_spec.t
		maktab $(SRC_HOME)/etc/ip_spec.t asstb.c

maktab:         $(SRC_DIR)/maktab.c $(SRC_DIR)/ip_spec.h
		$(UCC) $(ULDFLAGS) $(UCFLAGS) -o maktab $(SRC_DIR)/maktab.c $(UTIL_HOME)/lib.bin/em_data.$(ULIBSUF)

asprint:	$(SRC_DIR)/asprint.p
		apc -w -o asprint $(SRC_DIR)/asprint.p

opr:
		make pr ^ opr

pr:
		@pr $(SRC_DIR)/ass00.h $(SRC_DIR)/assex.h $(SRC_DIR)/ip_spec.h $(SRC_DIR)/ass?0.c $(SRC_DIR)/ass[rcd]?.c $(SRC_DIR)/maktab.c

depend:	asstb.c
	sed '/^#DEPENDENCIES/,$$d' Makefile >Makefile.new
	echo '#DEPENDENCIES' >>Makefile.new
	for i in $(CFILES) ; do \
		echo "`basename $$i .c`.$$(SUF):	$$i" >> Makefile.new ; \
		echo '	$$(CC) -c $$(CFLAGS)' $$i >> Makefile.new ; \
		$(UTIL_HOME)/lib.bin/cpp -d $(INCLUDES) $$i | sed "s/^/`basename $$i .c`.$$(SUF):	/" >> Makefile.new ; \
	done
	mv Makefile Makefile.old
	mv Makefile.new Makefile

# do not remove the next line.
#DEPENDENCIES
ass00.$(SUF):	$(SRC_DIR)/ass00.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ass00.c
ass00.$(SUF):	$(SRC_DIR)/assex.h
ass00.$(SUF):	$(TARGET_HOME)/h/local.h
ass00.$(SUF):	$(TARGET_HOME)/h/arch.h
ass00.$(SUF):	$(TARGET_HOME)/h/em_flag.h
ass00.$(SUF):	$(TARGET_HOME)/h/as_spec.h
ass00.$(SUF):	$(TARGET_HOME)/h/em_spec.h
ass00.$(SUF):	$(SRC_DIR)/ass00.h
ass30.$(SUF):	$(SRC_DIR)/ass30.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ass30.c
ass30.$(SUF):	$(SRC_DIR)/ip_spec.h
ass30.$(SUF):	$(SRC_DIR)/assex.h
ass30.$(SUF):	$(TARGET_HOME)/h/local.h
ass30.$(SUF):	$(TARGET_HOME)/h/arch.h
ass30.$(SUF):	$(TARGET_HOME)/h/em_flag.h
ass30.$(SUF):	$(TARGET_HOME)/h/as_spec.h
ass30.$(SUF):	$(TARGET_HOME)/h/em_spec.h
ass30.$(SUF):	$(SRC_DIR)/ass00.h
ass40.$(SUF):	$(SRC_DIR)/ass40.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ass40.c
ass40.$(SUF):	$(SRC_DIR)/assex.h
ass40.$(SUF):	$(TARGET_HOME)/h/local.h
ass40.$(SUF):	$(TARGET_HOME)/h/arch.h
ass40.$(SUF):	$(TARGET_HOME)/h/em_flag.h
ass40.$(SUF):	$(TARGET_HOME)/h/as_spec.h
ass40.$(SUF):	$(TARGET_HOME)/h/em_spec.h
ass40.$(SUF):	$(SRC_DIR)/ass00.h
ass50.$(SUF):	$(SRC_DIR)/ass50.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ass50.c
ass50.$(SUF):	$(SRC_DIR)/ip_spec.h
ass50.$(SUF):	$(SRC_DIR)/assex.h
ass50.$(SUF):	$(TARGET_HOME)/h/local.h
ass50.$(SUF):	$(TARGET_HOME)/h/arch.h
ass50.$(SUF):	$(TARGET_HOME)/h/em_flag.h
ass50.$(SUF):	$(TARGET_HOME)/h/as_spec.h
ass50.$(SUF):	$(TARGET_HOME)/h/em_spec.h
ass50.$(SUF):	$(SRC_DIR)/ass00.h
ass60.$(SUF):	$(SRC_DIR)/ass60.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ass60.c
ass60.$(SUF):	$(SRC_DIR)/ip_spec.h
ass60.$(SUF):	$(SRC_DIR)/assex.h
ass60.$(SUF):	$(TARGET_HOME)/h/local.h
ass60.$(SUF):	$(TARGET_HOME)/h/arch.h
ass60.$(SUF):	$(TARGET_HOME)/h/em_flag.h
ass60.$(SUF):	$(TARGET_HOME)/h/as_spec.h
ass60.$(SUF):	$(TARGET_HOME)/h/em_spec.h
ass60.$(SUF):	$(SRC_DIR)/ass00.h
ass70.$(SUF):	$(SRC_DIR)/ass70.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ass70.c
ass70.$(SUF):	$(SRC_DIR)/assex.h
ass70.$(SUF):	$(TARGET_HOME)/h/local.h
ass70.$(SUF):	$(TARGET_HOME)/h/arch.h
ass70.$(SUF):	$(TARGET_HOME)/h/em_flag.h
ass70.$(SUF):	$(TARGET_HOME)/h/as_spec.h
ass70.$(SUF):	$(TARGET_HOME)/h/em_spec.h
ass70.$(SUF):	$(SRC_DIR)/ass00.h
ass80.$(SUF):	$(SRC_DIR)/ass80.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ass80.c
ass80.$(SUF):	$(TARGET_HOME)/h/em_path.h
ass80.$(SUF):	$(SRC_DIR)/assex.h
ass80.$(SUF):	$(TARGET_HOME)/h/local.h
ass80.$(SUF):	$(TARGET_HOME)/h/arch.h
ass80.$(SUF):	$(TARGET_HOME)/h/em_flag.h
ass80.$(SUF):	$(TARGET_HOME)/h/as_spec.h
ass80.$(SUF):	$(TARGET_HOME)/h/em_spec.h
ass80.$(SUF):	$(SRC_DIR)/ass00.h
assci.$(SUF):	$(SRC_DIR)/assci.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/assci.c
assci.$(SUF):	$(TARGET_HOME)/h/em_ptyp.h
assci.$(SUF):	$(TARGET_HOME)/h/em_pseu.h
assci.$(SUF):	$(TARGET_HOME)/h/em_mes.h
assci.$(SUF):	$(SRC_DIR)/assex.h
assci.$(SUF):	$(TARGET_HOME)/h/local.h
assci.$(SUF):	$(TARGET_HOME)/h/arch.h
assci.$(SUF):	$(TARGET_HOME)/h/em_flag.h
assci.$(SUF):	$(TARGET_HOME)/h/as_spec.h
assci.$(SUF):	$(TARGET_HOME)/h/em_spec.h
assci.$(SUF):	$(SRC_DIR)/ass00.h
assda.$(SUF):	$(SRC_DIR)/assda.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/assda.c
assda.$(SUF):	$(SRC_DIR)/assex.h
assda.$(SUF):	$(TARGET_HOME)/h/local.h
assda.$(SUF):	$(TARGET_HOME)/h/arch.h
assda.$(SUF):	$(TARGET_HOME)/h/em_flag.h
assda.$(SUF):	$(TARGET_HOME)/h/as_spec.h
assda.$(SUF):	$(TARGET_HOME)/h/em_spec.h
assda.$(SUF):	$(SRC_DIR)/ass00.h
assrl.$(SUF):	$(SRC_DIR)/assrl.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/assrl.c
assrl.$(SUF):	$(SRC_DIR)/assex.h
assrl.$(SUF):	$(TARGET_HOME)/h/local.h
assrl.$(SUF):	$(TARGET_HOME)/h/arch.h
assrl.$(SUF):	$(TARGET_HOME)/h/em_flag.h
assrl.$(SUF):	$(TARGET_HOME)/h/as_spec.h
assrl.$(SUF):	$(TARGET_HOME)/h/em_spec.h
assrl.$(SUF):	$(SRC_DIR)/ass00.h
asstb.$(SUF):	asstb.c
	$(CC) -c $(CFLAGS) asstb.c
asscm.$(SUF):	$(SRC_DIR)/asscm.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/asscm.c
asscm.$(SUF):	$(TARGET_HOME)/h/local.h
asscm.$(SUF):	$(TARGET_HOME)/h/arch.h
asscm.$(SUF):	$(TARGET_HOME)/h/em_flag.h
asscm.$(SUF):	$(TARGET_HOME)/h/as_spec.h
asscm.$(SUF):	$(TARGET_HOME)/h/em_spec.h
asscm.$(SUF):	$(SRC_DIR)/ass00.h
