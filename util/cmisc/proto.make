# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/cmisc
TARGET_BIN = $(TARGET_HOME)/bin
TARGET_MAN = $(TARGET_HOME)/man
CFLAGS = $(COPTIONS)
LDFLAGS = $(LDOPTIONS)
LINTFLAGS = $(LINTOPTIONS)

all:		cid cclash prid tabgen

install:	all
		cp cid cclash prid tabgen $(TARGET_BIN)
		cp $(SRC_DIR)/cid.1 $(SRC_DIR)/cclash.1 $(SRC_DIR)/prid.1 $(SRC_DIR)/tabgen.1 $(TARGET_MAN)

cmp:		all
		-cmp cid $(TARGET_BIN)/cid
		-cmp cclash $(TARGET_BIN)/cclash
		-cmp prid $(TARGET_BIN)/prid
		-cmp tabgen $(TARGET_BIN)/tabgen
		-cmp $(SRC_DIR)/cid.1 $(TARGET_MAN)/cid.1
		-cmp $(SRC_DIR)/cclash.1 $(TARGET_MAN)/cclash.1
		-cmp $(SRC_DIR)/prid.1 $(TARGET_MAN)/prid.1
		-cmp $(SRC_DIR)/tabgen.1 $(TARGET_MAN)/tabgen.1

clean:
		rm -f *.$(SUF) cid cclash prid tabgen

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/cclash.c $(SRC_DIR)/cid.c \
		$(SRC_DIR)/prid.c $(SRC_DIR)/GCIPM.c $(SRC_DIR)/tabgen.c

opr:
		make pr | opr

tabgen:		tabgen.$(SUF)
		$(CC) $(LDFLAGS) -o tabgen tabgen.$(SUF)

cid:		cid.$(SUF) GCIPM.$(SUF)
		$(CC) $(LDFLAGS) -o cid cid.$(SUF) GCIPM.$(SUF)

cclash:		cclash.$(SUF) GCIPM.$(SUF)
		$(CC) $(LDFLAGS) -o cclash cclash.$(SUF) GCIPM.$(SUF)

prid:		prid.$(SUF) GCIPM.$(SUF)
		$(CC) $(LDFLAGS) -o prid prid.$(SUF) GCIPM.$(SUF)

lint:
		$(LINT) $(LINTFLAGS) $(SRC_DIR)/cid.c $(SRC_DIR)/GCIPM.c
		$(LINT) $(LINTFLAGS) $(SRC_DIR)/prid.c $(SRC_DIR)/GCIPM.c
		$(LINT) $(LINTFLAGS) $(SRC_DIR)/cclash.c $(SRC_DIR)/GCIPM.c
		$(LINT) $(LINTFLAGS) $(SRC_DIR)/tabgen.c

tabgen.$(SUF):	$(SRC_DIR)/tabgen.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/tabgen.c

cid.$(SUF):	$(SRC_DIR)/cid.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/cid.c

prid.$(SUF):	$(SRC_DIR)/prid.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/prid.c

cclash.$(SUF):	$(SRC_DIR)/cclash.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/cclash.c

GCIPM.$(SUF):	$(SRC_DIR)/GCIPM.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/GCIPM.c
