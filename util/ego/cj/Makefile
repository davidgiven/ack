
EMH=../../../h
EML=../../../lib
SHARE=../share
OBJECTS=cj.o
SHOBJECTS=$(SHARE)/get.o $(SHARE)/put.o $(SHARE)/alloc.o $(SHARE)/global.o $(SHARE)/debug.o $(SHARE)/files.o $(SHARE)/map.o $(SHARE)/lset.o $(SHARE)/cset.o $(SHARE)/aux.o $(SHARE)/stack_chg.o $(SHARE)/go.o
SRC=cj.c

all:	$(OBJECTS)
cj: \
	$(OBJECTS) $(SHOBJECTS)
	 $(CC) -o cj -i $(OBJECTS) $(SHOBJECTS) $(EML)/em_data.a
lpr:
	pr $(SRC) | lpr
# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
cj.o:	../../../h/em_mnem.h
cj.o:	../../../h/em_spec.h
cj.o:	../share/alloc.h
cj.o:	../share/aux.h
cj.o:	../share/debug.h
cj.o:	../share/def.h
cj.o:	../share/files.h
cj.o:	../share/get.h
cj.o:	../share/global.h
cj.o:	../share/go.h
cj.o:	../share/lset.h
cj.o:	../share/map.h
cj.o:	../share/put.h
cj.o:	../share/stack_chg.h
cj.o:	../share/types.h
