EMH=../../../h
EML=../../../lib
SHARE=../share
CFLAGS=-DVERBOSE
OBJECTS=bo.o
SHOBJECTS=$(SHARE)/get.o $(SHARE)/put.o $(SHARE)/alloc.o $(SHARE)/global.o $(SHARE)/debug.o $(SHARE)/files.o $(SHARE)/map.o $(SHARE)/lset.o $(SHARE)/cset.o $(SHARE)/aux.o $(SHARE)/stack_chg.o $(SHARE)/go.o
SRC=bo.c 

all:	$(OBJECTS)
bo: \
	$(OBJECTS) $(SHOBJECTS)
	 $(CC) -o bo -i $(OBJECTS) $(SHOBJECTS) $(EML)/em_data.a
lpr:
	pr $(SRC) | lpr
# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
bo.o:	../share/alloc.h
bo.o:	../share/aux.h
bo.o:	../share/debug.h
bo.o:	../share/def.h
bo.o:	../share/files.h
bo.o:	../share/get.h
bo.o:	../share/global.h
bo.o:	../share/go.h
bo.o:	../share/lset.h
bo.o:	../share/map.h
bo.o:	../share/put.h
bo.o:	../share/types.h
bo.o:	../../../h/em_mnem.h
bo.o:	../../../h/em_pseu.h
bo.o:	../../../h/em_spec.h
