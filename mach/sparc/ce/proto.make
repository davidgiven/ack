# $Id$

#PARAMS		do not remove this line!

MACH = sparc
OBJ = as
SRC_DIR = $(SRC_HOME)/mach/$(MACH)/ce

CEG = $(TARGET_HOME)/lib.bin/ceg/util

all:
	( cd $(SRC_DIR); make CPP=$(UTIL_HOME)/lib.bin/cpp )
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH)

install:
	( cd $(SRC_DIR); make CPP=$(UTIL_HOME)/lib.bin/cpp )
	-mkdir $(TARGET_HOME)/lib.bin/sparc
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH) install

cmp:
	( cd $(SRC_DIR); make CPP=$(UTIL_HOME)/lib.bin/cpp )
	-make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH) cmp

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/EM_table.x $(SRC_DIR)/mach.h \
	    $(SRC_DIR)/mach.c $(SRC_DIR)/cache.c.x

opr:
	make pr | opr

# total cleanup
clean:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) clean

# only remove ce, ceg, and back directories
dclean:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) dclean
