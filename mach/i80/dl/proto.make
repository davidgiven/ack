# $Header$

#PARAMS		do not remove this line!

OBJLIB=$(TARGET_HOME)/modules/lib/libobject.$(LIBSUF)

SRC_DIR = $(SRC_HOME)/mach/i80/dl

all:	mccpm nascom

mccpm:	mccpm.$(SUF)
	$(CC) $(LDOPTIONS) -o mccpm mccpm.$(SUF) $(OBJLIB)

nascom:	nascom.$(SUF)
	$(CC) $(LDOPTIONS) -o nascom nascom.$(SUF) $(OBJLIB)

mccpm.$(SUF):	$(SRC_DIR)/mccpm.c
	$(CC) $(COPTIONS) -I$(TARGET_HOME)/h -c $(SRC_DIR)/mccpm.c

nascom.$(SUF):	$(SRC_DIR)/nascom.c
	$(CC) $(COPTIONS) -I$(TARGET_HOME)/h -c $(SRC_DIR)/nascom.c

install:	all
	echo Nothing is installed

cmp:	all
	echo Nothing is compared

clean:
	rm -f *.(SUF) Out

lint:
	$(LINT) $(LINTOPTIONS) -I$(TARGET_HOME)/h $(SRC_DIR)/mccpm.c $(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)
	$(LINT) $(LINTOPTIONS) -I$(TARGET_HOME)/h $(SRC_DIR)/nascom.c $(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/mccpm.c $(SRC_DIR)/nascom.c

opr:
	make pr | opr
