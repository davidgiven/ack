# $Header$

#PARAMS		do not remove this line!

SRC_DIR=$(SRC_HOME)/mach/sparc/ce_cg
MACH=sparc_solaris
LIB_DIR=$(TARGET_HOME)/modules/lib
EM_LIB=$(TARGET_HOME)/lib.bin
INCLUDE=-I$(TARGET_HOME)/modules/h -I$(TARGET_HOME)/h
CE_A= $(TARGET_HOME)/lib.bin/$(MACH)/ce.$(LIBSUF)
BACK_A= $(TARGET_HOME)/lib.bin/$(MACH)/back.$(LIBSUF)

CFLAGS=$(INCLUDE) -DCODE_EXPANDER $(COPTIONS)
LDFLAGS=$(LDOPTIONS)
LDLIBS=$(LIB_DIR)/libread_emk.$(LIBSUF) $(CE_A) $(BACK_A) \
	$(LIB_DIR)/libprint.$(LIBSUF) $(LIB_DIR)/libstring.$(LIBSUF) \
	$(LIB_DIR)/liballoc.$(LIBSUF) $(LIB_DIR)/libsystem.$(LIBSUF) \
	$(EM_LIB)/em_data.$(LIBSUF) $(LIB_DIR)/libflt.$(LIBSUF)

all:	cg

install:	all
	-mkdir $(TARGET_HOME)/lib.bin/$(MACH)
	cp cg $(TARGET_HOME)/lib.bin/$(MACH)/cg

cmp:	all
	cmp cp cg $(TARGET_HOME)/lib.bin/$(MACH)/cg

clean:
	rm -f *.$(SUF) cg

cg:	convert.$(SUF) $(CE_A) $(BACK_A)
	$(CC) $(LDFLAGS) -o cg convert.$(SUF) $(LDLIBS) 

convert.$(SUF):	$(SRC_DIR)/convert.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/convert.c

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/convert.c

opr:
	make pr | opr
