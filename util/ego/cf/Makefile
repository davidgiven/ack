EMH=../../../h
EML=../../../lib
CFLAGS=
SHARE=../share
CF=.
OBJECTS=cf.o cf_idom.o cf_loop.o cf_succ.o 
SHOBJECTS=$(SHARE)/get.o $(SHARE)/put.o $(SHARE)/alloc.o $(SHARE)/global.o $(SHARE)/debug.o $(SHARE)/files.o $(SHARE)/map.o $(SHARE)/lset.o $(SHARE)/cset.o $(SHARE)/aux.o
SRC=cf.h cf_succ.h cf_idom.h cf_loop.h cf.c cf_succ.c cf_idom.c cf_loop.c
all:	$(OBJECTS)
cf: \
	$(OBJECTS) $(SHOBJECTS)
	 $(CC) -o cf -i $(OBJECTS) $(SHOBJECTS) $(EML)/em_data.a
lpr:
	pr $(SRC) | lpr
# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
cf.o:	../../../h/em_mnem.h
cf.o:	../share/alloc.h
cf.o:	../share/cset.h
cf.o:	../share/debug.h
cf.o:	../share/files.h
cf.o:	../share/get.h
cf.o:	../share/global.h
cf.o:	../share/lset.h
cf.o:	../share/map.h
cf.o:	../share/put.h
cf.o:	../share/types.h
cf.o:	cf.h
cf.o:	cf_idom.h
cf.o:	cf_loop.h
cf.o:	cf_succ.h
cf_idom.o:	../share/alloc.h
cf_idom.o:	../share/debug.h
cf_idom.o:	../share/lset.h
cf_idom.o:	../share/types.h
cf_idom.o:	cf.h
cf_loop.o:	../share/alloc.h
cf_loop.o:	../share/debug.h
cf_loop.o:	../share/lset.h
cf_loop.o:	../share/types.h
cf_loop.o:	cf.h
cf_succ.o:	../../../h/em_flag.h
cf_succ.o:	../../../h/em_mnem.h
cf_succ.o:	../../../h/em_pseu.h
cf_succ.o:	../../../h/em_spec.h
cf_succ.o:	../share/cset.h
cf_succ.o:	../share/debug.h
cf_succ.o:	../share/def.h
cf_succ.o:	../share/global.h
cf_succ.o:	../share/lset.h
cf_succ.o:	../share/map.h
cf_succ.o:	../share/types.h
cf_succ.o:	cf.h
