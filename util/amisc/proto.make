# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/amisc
MANDIR	= $(TARGET_HOME)/man
BINDIR  = $(TARGET_HOME)/bin
LDFLAGS = $(LDOPTIONS)
INCLUDES = -I$(TARGET_HOME)/h
CFLAGS	= $(INCLUDES) $(COPTIONS)
LINTFLAGS = $(INCLUDES) $(LINTOPTIONS)

ALL	= anm asize astrip ashow
LIBS	= $(TARGET_HOME)/modules/lib/libobject.$(LIBSUF)
LINTLIBS = $(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)

all:	$(ALL)

anm:	$(SRC_DIR)/anm.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o anm $(SRC_DIR)/anm.c $(LIBS)
asize:	$(SRC_DIR)/asize.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o asize $(SRC_DIR)/asize.c $(LIBS)
astrip: $(SRC_DIR)/astrip.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o astrip $(SRC_DIR)/astrip.c $(LIBS)
ashow:	$(SRC_DIR)/ashow.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o ashow $(SRC_DIR)/ashow.c $(LIBS)

install:	all
	for i in $(ALL); do rm -f $(BINDIR)/$$i; cp $$i $(BINDIR)/$$i; done
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then for i in anm.1 asize.1 astrip.1; do rm -f $(MANDIR)/$$i; cp $(SRC_DIR)/$$i $(MANDIR)/$$i; done ; \
	fi

cmp:	all
	-for i in $(ALL); do cmp $$i $(BINDIR)/$$i; done
	-for i in anm.1 astrip.1 asize.1 ; do cmp $(SRC_DIR)/$$i $(MANDIR)/$$i; done

clean:	; rm -f $(ALL) *.$(SUF)

lint:
	$(LINT) $(LINTFLAGS) $(SRC_DIR)/anm.c $(LINTLIBS)
	$(LINT) $(LINTFLAGS) $(SRC_DIR)/asize.c $(LINTLIBS)
	$(LINT) $(LINTFLAGS) $(SRC_DIR)/astrip.c $(LINTLIBS)
	$(LINT) $(LINTFLAGS) $(SRC_DIR)/ashow.c $(LINTLIBS)

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/anm.c $(SRC_DIR)/astrip.c $(SRC_DIR)/asize.c $(SRC_DIR)/ashow.c

opr:
	make pr | opr
