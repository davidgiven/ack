# $Header$

#PARAMS		do not remove this line!

OBJLIB=$(TARGET_HOME)/modules/lib/libobject.$(LIBSUF)

SRC_DIR = $(SRC_HOME)/mach/xenix3/cv

all:	cv chstack

cv:	cv.$(SUF)
	$(CC) $(LDOPTIONS) -o cv cv.$(SUF) $(OBJLIB)

chstack:	chstack.$(SUF)
	$(CC) $(LDOPTIONS) -o chstack chstack.$(SUF)

cv.$(SUF):	$(SRC_DIR)/cv.c
	$(CC) $(COPTIONS) -I$(TARGET_HOME)/h -c $(SRC_DIR)/cv.c

chstack.$(SUF):	$(SRC_DIR)/chstack.c
	$(CC) $(COPTIONS) -c $(SRC_DIR)/chstack.c

install:	all
	-mkdir $(TARGET_HOME)/lib.bin/xenix3
	cp cv $(TARGET_HOME)/lib.bin/xenix3/cv
	cp chstack $(TARGET_HOME)/lib.bin/xenix3/chstack

cmp:	all
	-cmp cv $(TARGET_HOME)/lib.bin/xenix3/cv
	-cmp chstack $(TARGET_HOME)/lib.bin/xenix3/chstack

clean:
	rm -f *.$(SUF) Out cv chstack

lint:
	$(LINT) $(LINTOPTIONS) -I$(TARGET_HOME)/h $(SRC_DIR)/cv.c $(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)
	$(LINT) $(LINTOPTIONS) -I$(TARGET_HOME)/h $(SRC_DIR)/chstack.c

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/cv.c $(SRC_DIR)/chstack.c

opr:
	make pr | opr
