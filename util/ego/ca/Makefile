EMH=../../../h
EML=../../../lib
CFLAGS=
SHARE=../share
CA=.
OBJECTS=ca.o ca_put.o
SHOBJECTS=$(SHARE)/get.o $(SHARE)/alloc.o $(SHARE)/global.o $(SHARE)/aux.o $(SHARE)/debug.o $(SHARE)/lset.o $(SHARE)/cset.o $(SHARE)/files.o $(SHARE)/map.o
SRC=ca.h ca_put.h ca.c ca_put.c

all:	$(OBJECTS)
ca: \
	$(OBJECTS) $(SHOBJECTS)
	 $(CC) -o ca -i $(OBJECTS) $(SHOBJECTS) $(EML)/em_data.a
lpr:
	pr $(SRC) | lpr
# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
ca.o:	../share/alloc.h
ca.o:	../share/debug.h
ca.o:	../share/files.h
ca.o:	../share/get.h
ca.o:	../share/global.h
ca.o:	../share/lset.h
ca.o:	../share/map.h
ca.o:	../share/types.h
ca.o:	ca.h
ca.o:	ca_put.h
ca_put.o:	../../../h/em_flag.h
ca_put.o:	../../../h/em_mes.h
ca_put.o:	../../../h/em_mnem.h
ca_put.o:	../../../h/em_pseu.h
ca_put.o:	../../../h/em_spec.h
ca_put.o:	../share/alloc.h
ca_put.o:	../share/debug.h
ca_put.o:	../share/def.h
ca_put.o:	../share/map.h
ca_put.o:	../share/types.h
ca_put.o:	ca.h
