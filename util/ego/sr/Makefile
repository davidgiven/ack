
EMH=../../../h
EML=../../../lib
CFLAGS=-DVERBOSE
SHARE=../share
SR=.
OBJECTS=sr.o sr_expr.o sr_reduce.o sr_iv.o sr_cand.o sr_xform.o sr_aux.o 
SHOBJECTS=$(SHARE)/get.o $(SHARE)/put.o $(SHARE)/alloc.o $(SHARE)/global.o $(SHARE)/debug.o $(SHARE)/files.o $(SHARE)/map.o $(SHARE)/lset.o $(SHARE)/cset.o $(SHARE)/aux.o $(SHARE)/go.o
SRC=sr.h sr_iv.h sr_reduce.h sr_cand.h sr_xform.h sr_expr.h sr_aux.h sr.c sr_iv.c sr_reduce.c sr_cand.c sr_xform.c sr_expr.c sr_aux.c
.c.o:
	cc $(CFLAGS) -c $<
all:	$(OBJECTS)
sr: \
	$(OBJECTS) $(SHOBJECTS)
	 cc -o sr -i $(OBJECTS) $(SHOBJECTS) $(EML)/em_data.a
lpr:
	pr $(SRC) | lpr
opr:
	pr $(SRC) | opr
dumpflop:
	tar -uf /mnt/ego/sr/sr.tarf $(SRC) Makefile
# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
sr.o:	../share/alloc.h
sr.o:	../share/debug.h
sr.o:	../share/files.h
sr.o:	../share/get.h
sr.o:	../share/global.h
sr.o:	../share/lset.h
sr.o:	../share/map.h
sr.o:	../share/put.h
sr.o:	../share/types.h
sr.o:	sr.h
sr.o:	sr_aux.h
sr.o:	sr_iv.h
sr_aux.o:	../../../h/em_mnem.h
sr_aux.o:	../../../h/em_pseu.h
sr_aux.o:	../share/aux.h
sr_aux.o:	../share/debug.h
sr_aux.o:	../share/global.h
sr_aux.o:	../share/lset.h
sr_aux.o:	../share/types.h
sr_aux.o:	sr.h
sr_aux.o:	sr_aux.h
sr_aux.o:	sr_xform.h
sr_cand.o:	../../../h/em_mnem.h
sr_cand.o:	../../../h/em_pseu.h
sr_cand.o:	../share/aux.h
sr_cand.o:	../share/cset.h
sr_cand.o:	../share/debug.h
sr_cand.o:	../share/global.h
sr_cand.o:	../share/lset.h
sr_cand.o:	../share/map.h
sr_cand.o:	../share/types.h
sr_cand.o:	sr.h
sr_cand.o:	sr_aux.h
sr_cand.o:	sr_cand.h
sr_expr.o:	../../../h/em_mnem.h
sr_expr.o:	../share/aux.h
sr_expr.o:	../share/debug.h
sr_expr.o:	../share/global.h
sr_expr.o:	../share/lset.h
sr_expr.o:	../share/types.h
sr_expr.o:	sr.h
sr_expr.o:	sr_aux.h
sr_expr.o:	sr_iv.h
sr_iv.o:	../../../h/em_mnem.h
sr_iv.o:	../../../h/em_pseu.h
sr_iv.o:	../share/alloc.h
sr_iv.o:	../share/aux.h
sr_iv.o:	../share/cset.h
sr_iv.o:	../share/debug.h
sr_iv.o:	../share/global.h
sr_iv.o:	../share/lset.h
sr_iv.o:	../share/types.h
sr_iv.o:	sr.h
sr_iv.o:	sr_aux.h
sr_iv.o:	sr_cand.h
sr_iv.o:	sr_iv.h
sr_reduce.o:	../../../h/em_mes.h
sr_reduce.o:	../../../h/em_mnem.h
sr_reduce.o:	../../../h/em_pseu.h
sr_reduce.o:	../../../h/em_reg.h
sr_reduce.o:	../share/alloc.h
sr_reduce.o:	../share/aux.h
sr_reduce.o:	../share/debug.h
sr_reduce.o:	../share/global.h
sr_reduce.o:	../share/lset.h
sr_reduce.o:	../share/types.h
sr_reduce.o:	sr.h
sr_reduce.o:	sr_aux.h
sr_reduce.o:	sr_expr.h
sr_reduce.o:	sr_reduce.h
sr_reduce.o:	sr_xform.h
sr_xform.o:	../../../h/em_mnem.h
sr_xform.o:	../../../h/em_pseu.h
sr_xform.o:	../../../h/em_spec.h
sr_xform.o:	../share/alloc.h
sr_xform.o:	../share/aux.h
sr_xform.o:	../share/debug.h
sr_xform.o:	../share/def.h
sr_xform.o:	../share/get.h
sr_xform.o:	../share/global.h
sr_xform.o:	../share/lset.h
sr_xform.o:	../share/types.h
sr_xform.o:	sr.h
sr_xform.o:	sr_aux.h
sr_xform.o:	sr_xform.h
