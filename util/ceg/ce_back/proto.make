# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ceg/ce_back

CEG = $(TARGET_HOME)/lib.bin/ceg
BACK = $(CEG)/ce_back

ASLIST = back.h bottom.c bss.c con1.c con2.c con4.c \
		do_close.c do_open.c end_back.c gen1.c gen2.c gen4.c header.h \
		init_back.c reloc1.c reloc2.c reloc4.c rom1.c rom2.c rom4.c \
		set_global.c set_local.c switchseg.c symboldef.c text1.c \
		text2.c text4.c

OBJLIST = back.h con2.c con4.c data.c data.h do_close.c do_open.c \
		end_back.c extnd.c gen1.c gen2.c gen4.c hash.h header.h \
		init_back.c label.c memory.c misc.c output.c \
		reloc1.c reloc2.c reloc4.c relocation.c rom2.c rom4.c \
		set_global.c set_local.c switchseg.c symboldef.c symtable.c \
		text2.c text4.c common.c

all:

install:
	-mkdir $(CEG)
	-mkdir $(BACK)
	-mkdir $(BACK)/as_back
	-mkdir $(BACK)/obj_back
	for i in $(ASLIST) ; do cp $(SRC_DIR)/as_back/$$i $(BACK)/as_back/$$i ; done
	cp as_back/Makefile $(BACK)/as_back
	for i in $(OBJLIST) ; do cp $(SRC_DIR)/obj_back/$$i $(BACK)/obj_back/$$i ; done
	cp obj_back/Makefile $(BACK)/obj_back

cmp:
	-for i in $(ASLIST) ; do cmp $(SRC_DIR)/as_back/$$i $(BACK)/as_back/$$i ; done
	-cmp as_back/Makefile $(BACK)/as_back
	-for i in $(OBJLIST) ; do cmp $(SRC_DIR)/obj_back/$$i $(BACK)/obj_back/$$i ; done
	-cmp obj_back/Makefile $(BACK)/obj_back

pr:
	@for i in $(ASLIST) ; do pr $(SRC_DIR)/as_back/$$i ; done
	@for i in $(OBJLIST) ; do pr $(SRC_DIR)/obj_back/$$i ; done

opr:
	make pr | opr

clean:
