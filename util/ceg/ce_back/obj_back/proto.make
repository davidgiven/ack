# $Header$

#PARAMS		do not remove this line!

CEG=$(TARGET_HOME)/lib.bin/ceg
SOURCE=$(CEG)/ce_back/obj_back

IDIRS=-I.\
      -I$(SRC_DIR)\
      -I$(TARGET_HOME)/h\
      -I$(TARGET_HOME)/modules/h

CFLAGS = $(COPTIONS) $(IDIRS)

all:	data.$(SUF) con2.$(SUF) con4.$(SUF) relocation.$(SUF) end_back.$(SUF) \
	gen1.$(SUF) gen2.$(SUF) gen4.$(SUF) init_back.$(SUF) output.$(SUF) \
	reloc1.$(SUF) reloc2.$(SUF) reloc4.$(SUF) rom2.$(SUF) rom4.$(SUF) \
	set_global.$(SUF) set_local.$(SUF) switchseg.$(SUF) symboldef.$(SUF) \
	text2.$(SUF) text4.$(SUF) do_open.$(SUF) do_close.$(SUF) memory.$(SUF) \
	label.$(SUF) misc.$(SUF) extnd.$(SUF) symtable.$(SUF) common.$(SUF) \
	dbsym.$(SUF)

data.$(SUF) : data.h back.h header.h $(SOURCE)/data.c
	$(CC) $(CFLAGS) -c $(SOURCE)/data.c

memory.$(SUF) :data.h  back.h header.h $(SOURCE)/memory.c
	$(CC) $(CFLAGS) -c $(SOURCE)/memory.c

con2.$(SUF) : data.h back.h header.h $(SOURCE)/con2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/con2.c

con4.$(SUF) : data.h back.h header.h $(SOURCE)/con4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/con4.c

relocation.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/relocation.c
	$(CC) $(CFLAGS) -c $(SOURCE)/relocation.c

do_open.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/do_open.c
	$(CC) $(CFLAGS) -c $(SOURCE)/do_open.c

do_close.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/do_close.c
	$(CC) $(CFLAGS) -c $(SOURCE)/do_close.c

gen1.$(SUF) : data.h back.h header.h $(SOURCE)/gen1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/gen1.c

gen2.$(SUF) : data.h back.h header.h $(SOURCE)/gen2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/gen2.c

gen4.$(SUF) : data.h back.h header.h $(SOURCE)/gen4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/gen4.c

init_back.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/init_back.c
	$(CC) $(CFLAGS) -c $(SOURCE)/init_back.c

end_back.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/end_back.c
	$(CC) $(CFLAGS) -c $(SOURCE)/end_back.c

output.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/output.c
	$(CC) $(CFLAGS) -c $(SOURCE)/output.c

reloc1.$(SUF) : data.h back.h header.h $(SOURCE)/reloc1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/reloc1.c

reloc2.$(SUF) : data.h back.h header.h $(SOURCE)/reloc2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/reloc2.c

reloc4.$(SUF) : data.h back.h header.h $(SOURCE)/reloc4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/reloc4.c

rom2.$(SUF) : data.h back.h header.h $(SOURCE)/rom2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/rom2.c

rom4.$(SUF) : data.h back.h header.h $(SOURCE)/rom4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/rom4.c

set_global.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/set_global.c
	$(CC) $(CFLAGS) -c $(SOURCE)/set_global.c

set_local.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/set_local.c
	$(CC) $(CFLAGS) -c $(SOURCE)/set_local.c

switchseg.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/switchseg.c
	$(CC) $(CFLAGS) -c $(SOURCE)/switchseg.c

symboldef.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/symboldef.c
	$(CC) $(CFLAGS) -c $(SOURCE)/symboldef.c

text2.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/text2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/text2.c

text4.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/text4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/text4.c

symtable.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/symtable.c
	$(CC) $(CFLAGS) -c $(SOURCE)/symtable.c

extnd.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/extnd.c
	$(CC) $(CFLAGS) -c $(SOURCE)/extnd.c

misc.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/misc.c
	$(CC) $(CFLAGS) -c $(SOURCE)/misc.c

label.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/label.c
	$(CC) $(CFLAGS) -c $(SOURCE)/label.c

common.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/common.c
	$(CC) $(CFLAGS) -c $(SOURCE)/common.c

dbsym.$(SUF) : data.h back.h $(SRC_DIR)/mach.h $(SOURCE)/dbsym.c
	$(CC) $(CFLAGS) -c $(SOURCE)/dbsym.c
