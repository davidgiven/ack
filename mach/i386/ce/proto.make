# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/mach/i386/ce

CEG = $(TARGET_HOME)/lib.bin/ceg/util
OBJ = obj
MACH = i386

all:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH)

install:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH) install

cmp:
	-make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH) cmp

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/EM_table $(SRC_DIR)/mach.h $(SRC_DIR)/mach.c $(SRC_DIR)/as_table $(SRC_DIR)/as.h $(SRC_DIR)/as.c

opr:
	make pr | opr

# total cleanup
clean:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) clean

# only remove ce, ceg, and back directories
dclean:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) dclean
