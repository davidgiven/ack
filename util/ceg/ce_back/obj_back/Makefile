EM=/proj/em/Work
CEG=/usr/star/kaashoek/em/ceg
SOURCE=$(CEG)/ce_back/obj_back

IDIRS=-I.\
      -I..\
      -I$(EM)/h\
      -I$(EM)/modules/h

LIBS=$(EM)/modules/lib/object.a\
     $(EM)/modules/lib/libstring.a\
     $(EM)/modules/lib/libprint.a\
     $(EM)/modules/lib/liballoc.a\
     $(EM)/modules/lib/libsystem.a

all : data.o con2.o con4.o relocation.o end_back.o gen1.o gen2.o\
      gen4.o init_back.o mysprint.o output.o reloc1.o reloc2.o reloc4.o\
      rom2.o rom4.o set_global.o set_local.o switchseg.o symboldef.o text2.o\
      text4.o do_open.o do_close.o memory.o label.o misc.o extnd.o symtable.o

data.o : data.h back.h header.h $(SOURCE)/data.c
	cc -c $(IDIRS) $(SOURCE)/data.c

memory.o :data.h  back.h header.h $(SOURCE)/memory.c
	cc -c $(IDIRS) $(SOURCE)/memory.c

con2.o : data.h back.h header.h $(SOURCE)/con2.c
	cc -c $(IDIRS) $(SOURCE)/con2.c

con4.o : data.h back.h header.h $(SOURCE)/con4.c
	cc -c $(IDIRS) $(SOURCE)/con4.c

relocation.o : data.h back.h ../mach.h $(SOURCE)/relocation.c
	cc -c $(IDIRS) -I.. $(SOURCE)/relocation.c

do_open.o : data.h back.h ../mach.h $(SOURCE)/do_open.c
	cc -c $(IDIRS) -I.. $(SOURCE)/do_open.c

do_close.o : data.h back.h ../mach.h $(SOURCE)/do_close.c
	cc -c $(IDIRS) -I.. $(SOURCE)/do_close.c

gen1.o : data.h back.h header.h $(SOURCE)/gen1.c
	cc -c $(IDIRS) $(SOURCE)/gen1.c

gen2.o : data.h back.h header.h $(SOURCE)/gen2.c
	cc -c $(IDIRS) $(SOURCE)/gen2.c

gen4.o : data.h back.h header.h $(SOURCE)/gen4.c
	cc -c $(IDIRS) $(SOURCE)/gen4.c

init_back.o : data.h back.h ../mach.h $(SOURCE)/init_back.c
	cc -c $(IDIRS) -I.. $(SOURCE)/init_back.c

end_back.o : data.h back.h ../mach.h $(SOURCE)/end_back.c
	cc -c $(IDIRS) -I.. $(SOURCE)/end_back.c

mysprint.o : data.h back.h data.h $(SOURCE)/mysprint.c
	cc -c $(IDIRS) $(SOURCE)/mysprint.c

output.o : data.h back.h ../mach.h $(SOURCE)/output.c
	cc -c $(IDIRS) -I.. $(SOURCE)/output.c

reloc1.o : data.h back.h header.h $(SOURCE)/reloc1.c
	cc -c $(IDIRS) $(SOURCE)/reloc1.c

reloc2.o : data.h back.h header.h $(SOURCE)/reloc2.c
	cc -c $(IDIRS) $(SOURCE)/reloc2.c

reloc4.o : data.h back.h header.h $(SOURCE)/reloc4.c
	cc -c $(IDIRS) $(SOURCE)/reloc4.c

rom2.o : data.h back.h header.h $(SOURCE)/rom2.c
	cc -c $(IDIRS) $(SOURCE)/rom2.c

rom4.o : data.h back.h header.h $(SOURCE)/rom4.c
	cc -c $(IDIRS) $(SOURCE)/rom4.c

set_global.o : data.h back.h ../mach.h $(SOURCE)/set_global.c
	cc -c $(IDIRS) -I.. $(SOURCE)/set_global.c

set_local.o : data.h back.h ../mach.h $(SOURCE)/set_local.c
	cc -c $(IDIRS) -I.. $(SOURCE)/set_local.c

switchseg.o : data.h back.h ../mach.h $(SOURCE)/switchseg.c
	cc -c $(IDIRS) -I.. $(SOURCE)/switchseg.c

symboldef.o : data.h back.h ../mach.h $(SOURCE)/symboldef.c
	cc -c $(IDIRS) -I.. $(SOURCE)/symboldef.c

text2.o : data.h back.h ../mach.h $(SOURCE)/text2.c
	cc -c $(IDIRS) -I.. $(SOURCE)/text2.c

text4.o : data.h back.h ../mach.h $(SOURCE)/text4.c
	cc -c $(IDIRS) -I.. $(SOURCE)/text4.c

symtable.o : data.h back.h ../mach.h $(SOURCE)/symtable.c
	cc -c $(IDIRS) -I.. $(SOURCE)/symtable.c

extnd.o : data.h back.h ../mach.h $(SOURCE)/extnd.c
	cc -c $(IDIRS) -I.. $(SOURCE)/extnd.c

misc.o : data.h back.h ../mach.h $(SOURCE)/misc.c
	cc -c $(IDIRS) -I.. $(SOURCE)/misc.c

label.o : data.h back.h ../mach.h $(SOURCE)/label.c
	cc -c $(IDIRS) -I.. $(SOURCE)/label.c

