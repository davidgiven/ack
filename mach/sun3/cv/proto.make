# $Header$

#PARAMS		do not remove this line!

OBJLIB=$(TARGET_HOME)/modules/lib/libobject.$(LIBSUF)

SRC_DIR = $(SRC_HOME)/mach/sun3/cv

all:	cv Xcv

cv:	cv.$(SUF)
	$(CC) $(LDOPTIONS) -o cv cv.$(SUF) $(OBJLIB)

Xcv:	Xcv.$(SUF)
	$(CC) $(LDOPTIONS) -o Xcv Xcv.$(SUF) $(OBJLIB)

cv.$(SUF):	$(SRC_DIR)/cv.c
	$(CC) $(COPTIONS) -DMACH=2 -I$(TARGET_HOME)/h -c $(SRC_DIR)/cv.c

Xcv.$(SUF):	$(SRC_DIR)/Xcv.c
	$(CC) $(COPTIONS) -DMACH=2 -I$(TARGET_HOME)/h -c $(SRC_DIR)/Xcv.c

install:	all
	cp cv $(TARGET_HOME)/lib.bin/sun3/cv
	cp Xcv $(TARGET_HOME)/lib.bin/sun3/Xcv

cmp:	all
	-cmp cv $(TARGET_HOME)/lib.bin/sun3/cv
	-cmp Xcv $(TARGET_HOME)/lib.bin/sun3/Xcv

clean:
	rm -f *.(SUF) Out

lint:
	$(LINT) $(LINTOPTIONS) -DMACH=2 -I$(TARGET_HOME)/h $(SRC_DIR)/cv.c $(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)
	$(LINT) $(LINTOPTIONS) -DMACH=2 -I$(TARGET_HOME)/h $(SRC_DIR)/Xcv.c $(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/cv.c $(SRC_DIR)/Xcv.c

opr:
	make pr | opr
