EMH=../../../h
EML=../../../lib
CFLAGS=
DEBUG=../share
SHARE=../share
MALLOC=
IC=.
OBJECTS=ic.o ic_aux.o ic_lookup.o ic_io.o ic_lib.o
MOBJECTS=ic.m ic_aux.m ic_lookup.m ic_io.m ic_lib.m
SHOBJECTS=$(SHARE)/put.o $(SHARE)/alloc.o $(SHARE)/global.o $(SHARE)/debug.o $(SHARE)/files.o $(SHARE)/map.o $(SHARE)/lset.o $(SHARE)/cset.o $(SHARE)/aux.o
MSHOBJECTS=$(SHARE)/put.m $(SHARE)/alloc.m $(SHARE)/global.m $(SHARE)/debug.m $(SHARE)/files.m $(SHARE)/map.m $(SHARE)/lset.m $(SHARE)/cset.m
SRC=ic.h ic_aux.h ic_lib.h ic_lookup.h ic_io.h ic.c ic_aux.c ic_lib.c ic_lookup.c ic_io.c
.SUFFIXES: .m
.c.m:
	ack -O -L -c.m $(CFLAGS) $<
.c.o:
	cc $(CFLAGS) -c $<
all:	$(OBJECTS)
ic: \
	$(OBJECTS) $(SHOBJECTS)
	 cc -i -o ic $(OBJECTS) $(SHOBJECTS) $(EML)/em_data.a $(MALLOC)
optim:	$(MOBJECTS) $(MSHOBJECTS) 
	 ego IC CF $(F) CA $(MOBJECTS) $(MSHOBJECTS)
	 ack -O -o ic.ego -.c lfile.m $(EML)/em_data.a
	 
lpr:
	pr $(SRC) | lpr
dumpflop:
	tar -uf /mnt/ego/ic/ic.tarf $(SRC) Makefile
# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
ic.o:	../../../h/em_flag.h
ic.o:	../../../h/em_mes.h
ic.o:	../../../h/em_pseu.h
ic.o:	../../../h/em_spec.h
ic.o:	../share/alloc.h
ic.o:	../share/aux.h
ic.o:	../share/debug.h
ic.o:	../share/def.h
ic.o:	../share/files.h
ic.o:	../share/global.h
ic.o:	../share/map.h
ic.o:	../share/put.h
ic.o:	../share/types.h
ic.o:	ic.h
ic.o:	ic_aux.h
ic.o:	ic_io.h
ic.o:	ic_lib.h
ic.o:	ic_lookup.h
ic_aux.o:	../../../h/em_mnem.h
ic_aux.o:	../../../h/em_pseu.h
ic_aux.o:	../../../h/em_spec.h
ic_aux.o:	../share/alloc.h
ic_aux.o:	../share/aux.h
ic_aux.o:	../share/debug.h
ic_aux.o:	../share/def.h
ic_aux.o:	../share/global.h
ic_aux.o:	../share/types.h
ic_aux.o:	ic.h
ic_aux.o:	ic_aux.h
ic_aux.o:	ic_io.h
ic_aux.o:	ic_lookup.h
ic_io.o:	../../../h/em_pseu.h
ic_io.o:	../../../h/em_spec.h
ic_io.o:	../share/alloc.h
ic_io.o:	../share/debug.h
ic_io.o:	../share/types.h
ic_io.o:	ic.h
ic_io.o:	ic_io.h
ic_io.o:	ic_lookup.h
ic_lib.o:	../../../h/em_mes.h
ic_lib.o:	../../../h/em_pseu.h
ic_lib.o:	../../../h/em_spec.h
ic_lib.o:	../share/debug.h
ic_lib.o:	../share/files.h
ic_lib.o:	../share/global.h
ic_lib.o:	../share/types.h
ic_lib.o:	ic.h
ic_lib.o:	ic_io.h
ic_lib.o:	ic_lib.h
ic_lib.o:	ic_lookup.h
ic_lookup.o:	../../../h/em_spec.h
ic_lookup.o:	../share/alloc.h
ic_lookup.o:	../share/debug.h
ic_lookup.o:	../share/map.h
ic_lookup.o:	../share/types.h
ic_lookup.o:	ic.h
ic_lookup.o:	ic_lookup.h
