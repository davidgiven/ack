# $Header$

#PARAMS		do not remove this line!

CEG=$(TARGET_HOME)/lib.bin/ceg
SOURCE=$(CEG)/ce_back/as_back

IDIRS=-I.\
      -I$(SRC_DIR)\
      -I$(TARGET_HOME)/h\
      -I$(TARGET_HOME)/modules/h

CFLAGS = $(COPTIONS) $(IDIRS) -DNDEBUG

all:	bottom.$(SUF) con1.$(SUF) con2.$(SUF) con4.$(SUF) end_back.$(SUF) \
	gen1.$(SUF) gen2.$(SUF) gen4.$(SUF) init_back.$(SUF) reloc1.$(SUF) \
	reloc2.$(SUF) reloc4.$(SUF) bss.$(SUF) rom1.$(SUF) rom2.$(SUF) \
	rom4.$(SUF) set_global.$(SUF) set_local.$(SUF) switchseg.$(SUF) \
	symboldef.$(SUF) do_open.$(SUF) do_close.$(SUF) text1.$(SUF) \
	text2.$(SUF) text4.$(SUF) dbsym.$(SUF)

bottom.$(SUF) : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/bottom.c
	$(CC) $(CFLAGS) -c $(SOURCE)/bottom.c

bss.$(SUF) : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/bss.c
	$(CC) $(CFLAGS) -c $(SOURCE)/bss.c

text1.$(SUF) : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/text1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/text1.c

text2.$(SUF) : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/text2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/text2.c

text4.$(SUF) : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/text4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/text4.c

con1.$(SUF) : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/con1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/con1.c

con2.$(SUF) : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/con2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/con2.c

con4.$(SUF) : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/con4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/con4.c

do_open.$(SUF) : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/do_open.c
	$(CC) $(CFLAGS) -c $(SOURCE)/do_open.c

do_close.$(SUF) :  back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/do_close.c
	$(CC) $(CFLAGS) -c $(SOURCE)/do_close.c

gen1.$(SUF) : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/gen1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/gen1.c

gen2.$(SUF) : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/gen2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/gen2.c

gen4.$(SUF) : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/gen4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/gen4.c

init_back.$(SUF) : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/init_back.c
	$(CC) $(CFLAGS) -c $(SOURCE)/init_back.c

end_back.$(SUF) : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/end_back.c
	$(CC) $(CFLAGS) -c $(SOURCE)/end_back.c

reloc1.$(SUF) : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/reloc1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/reloc1.c

reloc2.$(SUF) : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/reloc2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/reloc2.c

reloc4.$(SUF) : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/reloc4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/reloc4.c

rom1.$(SUF) : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/rom1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/rom1.c

rom2.$(SUF) : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/rom2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/rom2.c

rom4.$(SUF) : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/rom4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/rom4.c

set_global.$(SUF) : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/set_global.c
	$(CC) $(CFLAGS) -c $(SOURCE)/set_global.c

set_local.$(SUF) : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/set_local.c
	$(CC) $(CFLAGS) -c $(SOURCE)/set_local.c

switchseg.$(SUF) : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/switchseg.c
	$(CC) $(CFLAGS) -c $(SOURCE)/switchseg.c

symboldef.$(SUF) : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/symboldef.c
	$(CC) $(CFLAGS) -c $(SOURCE)/symboldef.c

dbsym.$(SUF) : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/dbsym.c
	$(CC) $(CFLAGS) -c $(SOURCE)/dbsym.c
