# $Id$

#PARAMS		do not remove this line!

OBJLIB=$(TARGET_HOME)/modules/lib/libobject.$(LIBSUF)

SRC_DIR = $(SRC_HOME)/mach/vax4/cv

all:	cv

cv:	cv.$(SUF)
	$(CC) $(LDOPTIONS) -o cv cv.$(SUF) $(OBJLIB)

cv.$(SUF):	$(SRC_DIR)/cv.c
	$(CC) $(COPTIONS) -I$(TARGET_HOME)/h -c $(SRC_DIR)/cv.c

install:	all
	-mkdir $(TARGET_HOME)/lib.bin/vax4
	cp cv $(TARGET_HOME)/lib.bin/vax4/cv

cmp:	all
	-cmp cv $(TARGET_HOME)/lib.bin/vax4/cv

clean:
	rm -f *.$(SUF) Out cv

lint:
	$(LINT) $(LINTOPTIONS) -I$(TARGET_HOME)/h $(SRC_DIR)/cv.c $(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/cv.c

opr:
	make pr | opr
