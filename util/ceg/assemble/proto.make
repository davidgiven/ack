# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ceg/assemble

CEG = $(TARGET_HOME)/lib.bin/ceg
ASS = $(CEG)/assemble

ASLIST = assemble.c block_as.c
OBJLIST = assemble.c block_as.c const.h

all:

install:
	-mkdir $(CEG)
	-mkdir $(ASS)
	-mkdir $(ASS)/as_assemble
	-mkdir $(ASS)/obj_assemble
	for i in $(ASLIST) ; do cp $(SRC_DIR)/as_assemble/$$i $(ASS)/as_assemble/$$i ; done
	for i in $(OBJLIST) ; do cp $(SRC_DIR)/obj_assemble/$$i $(ASS)/obj_assemble/$$i ; done

cmp:
	-for i in $(ASLIST) ; do cmp $(SRC_DIR)/as_assemble/$$i $(ASS)/as_assemble/$$i ; done
	-for i in $(OBJLIST) ; do cmp $(SRC_DIR)/obj_assemble/$$i $(ASS)/obj_assemble/$$i ; done

pr:
	@for i in $(ASLIST) ; do pr $(SRC_DIR)/as_assemble/$$i ; done
	@for i in $(OBJLIST) ; do pr $(SRC_DIR)/obj_assemble/$$i ; done

opr:
	make pr | opr

clean:
