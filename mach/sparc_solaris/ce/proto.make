# $Id$

#PARAMS		do not remove this line!

MACH = sparc_solaris
OBJ = as
SRC_DIR = $(SRC_HOME)/mach/sparc/ce

CEG = $(TARGET_HOME)/lib.bin/ceg/util

all:
	( cd $(SRC_DIR); make )
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH) "SRC_DIR="$(SRC_DIR)

install:
	( cd $(SRC_DIR); make )
	-mkdir $(TARGET_HOME)/lib.bin/$(MACH)
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH) "SRC_DIR="$(SRC_DIR) install

cmp:
	( cd $(SRC_DIR); make )
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH) "SRC_DIR="$(SRC_DIR) cmp

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
