EM=../../..
CEG = $(EM)/lib/ceg
BACK = $(CEG)/ce_back

ASLIST = Makefile back.h bottom.c bss.c con1.c con2.c con4.c const.h \
		do_close.c do_open.c end_back.c gen1.c gen2.c gen4.c header.h \
		init_back.c reloc1.c reloc2.c reloc4.c rom1.c rom2.c rom4.c \
		set_global.c set_local.c switchseg.c symboldef.c text1.c \
		text2.c text4.c

OBJLIST = Makefile back.h con2.c con4.c data.c data.h do_close.c do_open.c \
		end_back.c extnd.c gen1.c gen2.c gen4.c hash.h header.h \
		init_back.c label.c memory.c misc.c mysprint.c output.c \
		reloc1.c reloc2.c reloc4.c relocation.c rom2.c rom4.c \
		set_global.c set_local.c switchseg.c symboldef.c symtable.c \
		text2.c text4.c common.c

all:

install:
	-mkdir $(CEG)
	-mkdir $(BACK)
	-mkdir $(BACK)/as_back
	-mkdir $(BACK)/obj_back
	for i in $(ASLIST) ; do cp as_back/$$i $(BACK)/as_back/$$i ; done
	for i in $(OBJLIST) ; do cp obj_back/$$i $(BACK)/obj_back/$$i ; done

cmp:
	-for i in $(ASLIST) ; do cmp as_back/$$i $(BACK)/as_back/$$i ; done
	-for i in $(OBJLIST) ; do cmp obj_back/$$i $(BACK)/obj_back/$$i ; done

pr:
	@for i in $(ASLIST) ; do pr as_back/$$i ; done
	@for i in $(OBJLIST) ; do pr obj_back/$$i ; done

opr:
	make pr | opr

clean:
