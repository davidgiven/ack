EMH=../../../h
EML=../../../lib
CFLAGS=
SHARE=../share
LV=.
OBJECTS=lv.o
SHOBJECTS=$(SHARE)/get.o $(SHARE)/aux.o $(SHARE)/put.o $(SHARE)/map.o $(SHARE)/alloc.o $(SHARE)/global.o $(SHARE)/debug.o $(SHARE)/lset.o $(SHARE)/cset.o $(SHARE)/parser.o $(SHARE)/files.o $(SHARE)/locals.o $(SHARE)/init_glob.o $(SHARE)/go.o
SRC=lv.h  lv.c 
.c.o:
	cc $(CFLAGS) -c $<
all:	$(OBJECTS)
lv: \
	$(OBJECTS) $(SHOBJECTS)
	 cc -o lv -i $(OBJECTS) $(SHOBJECTS) $(EML)/em_data.a
opr:
	pr $(SRC) | opr
dumpflop:
	tar -uf /mnt/ego/lv/lv.tarf $(SRC)
# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
lv.o:	../../../h/em_mnem.h
lv.o:	../../../h/em_pseu.h
lv.o:	../../../h/em_spec.h
lv.o:	../share/alloc.h
lv.o:	../share/aux.h
lv.o:	../share/cset.h
lv.o:	../share/debug.h
lv.o:	../share/def.h
lv.o:	../share/files.h
lv.o:	../share/get.h
lv.o:	../share/global.h
lv.o:	../share/go.h
lv.o:	../share/init_glob.h
lv.o:	../share/locals.h
lv.o:	../share/lset.h
lv.o:	../share/map.h
lv.o:	../share/parser.h
lv.o:	../share/put.h
lv.o:	../share/types.h
lv.o:	lv.h
