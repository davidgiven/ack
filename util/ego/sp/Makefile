
EMH=../../../h
EML=../../../lib
SHARE=../share
OBJECTS=sp.o
SHOBJECTS=$(SHARE)/get.o $(SHARE)/put.o $(SHARE)/alloc.o $(SHARE)/global.o $(SHARE)/debug.o $(SHARE)/files.o $(SHARE)/map.o $(SHARE)/lset.o $(SHARE)/cset.o $(SHARE)/aux.o $(SHARE)/stack_chg.o $(SHARE)/go.o
SRC=sp.c 
all:	$(OBJECTS)
sp: \
	$(OBJECTS) $(SHOBJECTS)
	 $(CC) -o sp -i $(OBJECTS) $(SHOBJECTS) $(EML)/em_data.a
lpr:
	pr $(SRC) | lpr
# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
sp.o:	../share/alloc.h
sp.o:	../share/aux.h
sp.o:	../share/debug.h
sp.o:	../share/files.h
sp.o:	../share/get.h
sp.o:	../share/global.h
sp.o:	../share/go.h
sp.o:	../share/lset.h
sp.o:	../share/map.h
sp.o:	../share/put.h
sp.o:	../share/stack_chg.h
sp.o:	../share/types.h
sp.o:	../../../h/em_mnem.h
sp.o:	../../../h/em_spec.h
stack_chg.o:	../share/debug.h
stack_chg.o:	../share/global.h
stack_chg.o:	../share/types.h
stack_chg.o:	../../../h/em_mnem.h
stack_chg.o:	../../../h/em_spec.h
stack_chg.o:	pop_push.h
