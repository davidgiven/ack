# $Header$

# requires a definition for TARGET_HOME, SRC_DIR, COPTIONS, CC

CEG=$(TARGET_HOME)/lib.bin/ceg
SOURCE=$(CEG)/ce_back/as_back

IDIRS=-I.\
      -I$(SRC_DIR)\
      -I..\
      -I$(TARGET_HOME)/h\
      -I$(TARGET_HOME)/modules/h

CFLAGS = $(COPTIONS) $(IDIRS)

all : bottom.o con1.o con2.o con4.o end_back.o gen1.o gen2.o\
      gen4.o init_back.o reloc1.o reloc2.o reloc4.o bss.o\
      rom1.o rom2.o rom4.o set_global.o set_local.o switchseg.o symboldef.o \
      do_open.o do_close.o text1.o text2.o text4.o con_str.o gen_str.o rom_str.o

bottom.o : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/bottom.c
	$(CC) $(CFLAGS) -c $(SOURCE)/bottom.c

bss.o : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/bss.c
	$(CC) $(CFLAGS) -c $(SOURCE)/bss.c

text1.o : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/text1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/text1.c

text2.o : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/text2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/text2.c

text4.o : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/text4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/text4.c

con1.o : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/con1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/con1.c

con2.o : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/con2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/con2.c

con4.o : $(SRC_DIR)/mach.h back.h header.h $(SOURCE)/con4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/con4.c

do_open.o : back.h header.h $(SRC_DIR)/mach.h do_open.c
	$(CC) $(CFLAGS) -c do_open.c

do_close.o :  back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/do_close.c
	$(CC) $(CFLAGS) -c $(SOURCE)/do_close.c

gen1.o : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/gen1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/gen1.c

gen2.o : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/gen2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/gen2.c

gen4.o : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/gen4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/gen4.c

init_back.o : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/init_back.c
	$(CC) $(CFLAGS) -c $(SOURCE)/init_back.c

end_back.o : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/end_back.c
	$(CC) $(CFLAGS) -c $(SOURCE)/end_back.c

reloc1.o : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/reloc1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/reloc1.c

reloc2.o : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/reloc2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/reloc2.c

reloc4.o : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/reloc4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/reloc4.c

rom1.o : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/rom1.c
	$(CC) $(CFLAGS) -c $(SOURCE)/rom1.c

rom2.o : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/rom2.c
	$(CC) $(CFLAGS) -c $(SOURCE)/rom2.c

rom4.o : back.h header.h $(SRC_DIR)/mach.h $(SOURCE)/rom4.c
	$(CC) $(CFLAGS) -c $(SOURCE)/rom4.c

set_global.o : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/set_global.c
	$(CC) $(CFLAGS) -c $(SOURCE)/set_global.c

set_local.o : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/set_local.c
	$(CC) $(CFLAGS) -c $(SOURCE)/set_local.c

switchseg.o : header.h back.h $(SRC_DIR)/mach.h $(SOURCE)/switchseg.c
	$(CC) $(CFLAGS) -c $(SOURCE)/switchseg.c

symboldef.o : header.h back.h $(SRC_DIR)/mach.h symboldef.c
	$(CC) $(CFLAGS) -c symboldef.c

con_str.o : header.h back.h $(SRC_DIR)/mach.h con_str.c
	$(CC) $(CFLAGS) -c con_str.c

gen_str.o : header.h back.h $(SRC_DIR)/mach.h gen_str.c
	$(CC) $(CFLAGS) -c gen_str.c

rom_str.o : header.h back.h $(SRC_DIR)/mach.h rom_str.c
	$(CC) $(CFLAGS) -c rom_str.c
