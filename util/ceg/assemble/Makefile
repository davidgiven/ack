EM=../../..
CEG = $(EM)/lib/ceg
ASS = $(CEG)/assemble

ASLIST = assemble.c block_as.c
OBJLIST = assemble.c block_as.c const.h

all:

install:
	-mkdir $(CEG)
	-mkdir $(ASS)
	-mkdir $(ASS)/as_assemble
	-mkdir $(ASS)/obj_assemble
	for i in $(ASLIST) ; do cp as_assemble/$$i $(ASS)/as_assemble/$$i ; done
	for i in $(OBJLIST) ; do cp obj_assemble/$$i $(ASS)/obj_assemble/$$i ; done

cmp:
	-for i in $(ASLIST) ; do cp as_assemble/$$i $(ASS)/as_assemble/$$i ; done
	-for i in $(OBJLIST) ; do cp obj_assemble/$$i $(ASS)/obj_assemble/$$i ; done

pr:
	@for i in $(ASLIST) ; do pr as_assemble/$$i ; done
	@for i in $(OBJLIST) ; do pr obj_assemble/$$i ; done

opr:
	make pr | opr

clean:
