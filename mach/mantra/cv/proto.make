# $Id$

#PARAMS		do not remove this line!

OBJLIB=$(TARGET_HOME)/modules/lib/libobject.$(LIBSUF)

SRC_DIR = $(SRC_HOME)/mach/mantra/cv

all:	cv Xcv

cv:	cv.$(SUF)
	$(CC) $(LDOPTIONS) -o cv cv.$(SUF) $(OBJLIB)

Xcv:	Xcv.$(SUF)
	$(CC) $(LDOPTIONS) -o Xcv Xcv.$(SUF) $(OBJLIB)

cv.$(SUF):	$(SRC_DIR)/cv.c
	$(CC) $(COPTIONS) -I$(TARGET_HOME)/h -c $(SRC_DIR)/cv.c

Xcv.$(SUF):	$(SRC_DIR)/Xcv.c
	$(CC) $(COPTIONS) -I$(TARGET_HOME)/h -c $(SRC_DIR)/Xcv.c

install:	all
	-mkdir $(TARGET_HOME)/lib.bin/mantra
	cp cv $(TARGET_HOME)/lib.bin/mantra/cv
	cp Xcv $(TARGET_HOME)/lib.bin/mantra/Xcv

cmp:	all
	-cmp cv $(TARGET_HOME)/lib.bin/mantra/cv
	-cmp Xcv $(TARGET_HOME)/lib.bin/mantra/Xcv

clean:
	rm -f *.$(SUF) Out cv Xcv

lint:
	$(LINT) $(LINTOPTIONS) -I$(TARGET_HOME)/h $(SRC_DIR)/cv.c $(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)
	$(LINT) $(LINTOPTIONS) -I$(TARGET_HOME)/h $(SRC_DIR)/Xcv.c $(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/cv.c $(SRC_DIR)/Xcv.c

opr:
	make pr | opr
