EM=/proj/em/Work
CEG=/usr/star/kaashoek/em/ceg
SOURCE=$(CEG)/ce_back/as_back

IDIRS=-I.\
      -I..\
      -I$(EM)/h\
      -I$(EM)/modules/h

LIBS=$(EM)/modules/lib/object.a\
     $(EM)/modules/lib/libstring.a\
     $(EM)/modules/lib/libprint.a\
     $(EM)/modules/lib/libsystem.a

all : bottom.o con1.o con2.o con4.o end_back.o gen1.o gen2.o\
      gen4.o init_back.o reloc1.o reloc2.o reloc4.o bss.o\
      rom1.o rom2.o rom4.o set_global.o set_local.o switchseg.o symboldef.o \
      do_open.o do_close.o text1.o text2.o text4.o

bottom.o : ../mach.h back.h header.h $(SOURCE)/bottom.c
	cc -c $(IDIRS) $(SOURCE)/bottom.c

bss.o : ../mach.h back.h header.h $(SOURCE)/bss.c
	cc -c $(IDIRS) $(SOURCE)/bss.c

text1.o : ../mach.h back.h header.h $(SOURCE)/text1.c
	cc -c $(IDIRS) $(SOURCE)/text1.c

text2.o : ../mach.h back.h header.h $(SOURCE)/text2.c
	cc -c $(IDIRS) $(SOURCE)/text2.c

text4.o : ../mach.h back.h header.h $(SOURCE)/text4.c
	cc -c $(IDIRS) $(SOURCE)/text4.c

con1.o : ../mach.h back.h header.h $(SOURCE)/con1.c
	cc -c $(IDIRS) $(SOURCE)/con1.c

con2.o : ../mach.h back.h header.h $(SOURCE)/con2.c
	cc -c $(IDIRS) $(SOURCE)/con2.c

con4.o : ../mach.h back.h header.h $(SOURCE)/con4.c
	cc -c $(IDIRS) $(SOURCE)/con4.c

do_open.o : back.h header.h ../mach.h $(SOURCE)/do_open.c
	cc -c $(IDIRS) -I.. $(SOURCE)/do_open.c

do_close.o :  back.h header.h ../mach.h $(SOURCE)/do_close.c
	cc -c $(IDIRS) -I.. $(SOURCE)/do_close.c

gen1.o : back.h header.h ../mach.h $(SOURCE)/gen1.c
	cc -c $(IDIRS) $(SOURCE)/gen1.c

gen2.o : back.h header.h ../mach.h $(SOURCE)/gen2.c
	cc -c $(IDIRS) $(SOURCE)/gen2.c

gen4.o : back.h header.h ../mach.h $(SOURCE)/gen4.c
	cc -c $(IDIRS) $(SOURCE)/gen4.c

init_back.o : header.h back.h ../mach.h $(SOURCE)/init_back.c
	cc -c $(IDIRS) -I.. $(SOURCE)/init_back.c

end_back.o : header.h back.h ../mach.h $(SOURCE)/end_back.c
	cc -c $(IDIRS) -I.. $(SOURCE)/end_back.c

reloc1.o : back.h header.h ../mach.h $(SOURCE)/reloc1.c
	cc -c $(IDIRS) $(SOURCE)/reloc1.c

reloc2.o : back.h header.h ../mach.h $(SOURCE)/reloc2.c
	cc -c $(IDIRS) $(SOURCE)/reloc2.c

reloc4.o : back.h header.h ../mach.h $(SOURCE)/reloc4.c
	cc -c $(IDIRS) $(SOURCE)/reloc4.c

rom1.o : back.h header.h ../mach.h $(SOURCE)/rom1.c
	cc -c $(IDIRS) $(SOURCE)/rom1.c

rom2.o : back.h header.h ../mach.h $(SOURCE)/rom2.c
	cc -c $(IDIRS) $(SOURCE)/rom2.c

rom4.o : back.h header.h ../mach.h $(SOURCE)/rom4.c
	cc -c $(IDIRS) $(SOURCE)/rom4.c

set_global.o : header.h back.h ../mach.h $(SOURCE)/set_global.c
	cc -c $(IDIRS) -I.. $(SOURCE)/set_global.c

set_local.o : header.h back.h ../mach.h $(SOURCE)/set_local.c
	cc -c $(IDIRS) -I.. $(SOURCE)/set_local.c

switchseg.o : header.h back.h ../mach.h $(SOURCE)/switchseg.c
	cc -c $(IDIRS) -I.. $(SOURCE)/switchseg.c

symboldef.o : header.h back.h ../mach.h $(SOURCE)/symboldef.c
	cc -c $(IDIRS) -I.. $(SOURCE)/symboldef.c
