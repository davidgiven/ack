# $Header$

#PARAMS		do not remove this line!

OBJLIB=$(TARGET_HOME)/modules/lib/libobject.$(LIBSUF)

SRC_DIR = $(SRC_HOME)/mach/m68k2/dl

all:	dl

dl:	dl.$(SUF)
	$(CC) $(LDOPTIONS) -o dl dl.$(SUF) $(OBJLIB)

dl.$(SUF):	$(SRC_DIR)/dl.c
	$(CC) $(COPTIONS) -I$(TARGET_HOME)/h -c $(SRC_DIR)/dl.c

install:	all
	echo Nothing is installed

cmp:	all
	echo Nothing is compared

clean:
	rm -f *.(SUF) Out

lint:
	$(LINT) $(LINTOPTIONS) -I$(TARGET_HOME)/h $(SRC_DIR)/dl.c $(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/dl.c

opr:
	make pr | opr
