# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/fast/driver
INCLUDES = -I$(TARGET_HOME)/config
CFLAGS = $(COPTIONS) $(INCLUDES)
LINTFLAGS = $(LINTOPTIONS) $(INCLUDES)
LDFLAGS = $(LDOPTIONS)

all:	afcc afm2 afpc

install:	all
	cp afcc afm2 afpc $(TARGET_HOME)/bin
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	cp $(SRC_DIR)/afcc.1 $(SRC_DIR)/afm2.1 $(SRC_DIR)/afpc.1 $(TARGET_HOME)/man ; \
	fi

cmp:	all
	-cmp afcc $(TARGET_HOME)/bin/afcc
	-cmp afm2 $(TARGET_HOME)/bin/afm2
	-cmp afpc $(TARGET_HOME)/bin/afpc
	-cmp afcc.1 $(TARGET_HOME)/man/afcc.1
	-cmp afm2.1 $(TARGET_HOME)/man/afm2.1
	-cmp afpc.1 $(TARGET_HOME)/man/afpc.1

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/driver.c

opr:
	make pr | opr

clean:
	rm -f *,$(SUF) afcc afm2 afpc Out

afcc.$(SUF):	$(SRC_DIR)/driver.c $(TARGET_HOME)/config/em_path.h
	$(CC) $(CFLAGS) -c -DFCC -D$(MACH) -D`ack_sys` $(SRC_DIR)/driver.c
	mv driver.$(SUF) afcc.$(SUF)

afpc.$(SUF):	$(SRC_DIR)/driver.c $(TARGET_HOME)/config/em_path.h
	$(CC) $(CFLAGS) -c -DFPC -D$(MACH) -D`ack_sys` $(SRC_DIR)/driver.c
	mv driver.$(SUF) afpc.$(SUF)

afm2.$(SUF):	$(SRC_DIR)/driver.c $(TARGET_HOME)/config/em_path.h
	$(CC) $(CFLAGS) -c -DFM2 -D$(MACH) -D`ack_sys` $(SRC_DIR)/driver.c
	mv driver.$(SUF) afm2.$(SUF)

afcc:	afcc.$(SUF)
	$(CC) $(LDFLAGS) -o afcc afcc.$(SUF)

afm2:	afm2.$(SUF)
	$(CC) $(LDFLAGS) -o afm2 afm2.$(SUF)

afpc:	afpc.$(SUF)
	$(CC) $(LDFLAGS) -o afpc afpc.$(SUF)
