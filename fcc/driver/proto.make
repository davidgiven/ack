# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/fcc/driver
INCLUDES = -I$(TARGET_HOME)/config
CFLAGS = $(COPTIONS) $(INCLUDES)
LINTFLAGS = $(LINTOPTIONS) $(INCLUDES)
LDFLAGS = $(LDOPTIONS)

all:	fcc

install:	all
	cp fcc $(TARGET_HOME)/bin
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	mk_manpage $(SRC_DIR)/fcc.1 $(TARGET_HOME) ; \
	fi

cmp:	all
	-cmp fcc $(TARGET_HOME)/bin/fcc

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/fcc.c

opr:
	make pr | opr

clean:
	rm -f *.$(SUF) fcc Out

lint:
	$(LINT) $(LINTFLAGS) -D`ack_sys` $(SRC_DIR)/driver.c

fcc.$(SUF):	$(SRC_DIR)/fcc.c $(TARGET_HOME)/config/em_path.h
	$(CC) $(CFLAGS) -c -D$(MACH) -D`ack_sys` $(SRC_DIR)/fcc.c

fcc:	fcc.$(SUF)
	$(CC) $(LDFLAGS) -o fcc fcc.$(SUF)
