EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH)
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
sr.c sr_iv.c sr_reduce.c sr_cand.c sr_xform.c sr_expr.c sr_aux.c

OFILES=\
sr.o sr_expr.o sr_reduce.o sr_iv.o sr_cand.o sr_xform.o sr_aux.o 

HFILES=\
sr.h sr_iv.h sr_reduce.h sr_cand.h sr_xform.h sr_expr.h sr_aux.h

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/get.o $(SHR)/put.o $(SHR)/alloc.o $(SHR)/global.o $(SHR)/debug.o \
$(SHR)/files.o $(SHR)/map.o $(SHR)/lset.o $(SHR)/cset.o $(SHR)/aux.o \
$(SHR)/go.o

SHARE_MFILES=\
$(SHR)/get.m $(SHR)/put.m $(SHR)/alloc.m $(SHR)/global.m $(SHR)/debug.m \
$(SHR)/files.m $(SHR)/map.m $(SHR)/lset.m $(SHR)/cset.m $(SHR)/aux.m \
$(SHR)/go.m

all:	sr

sr:	$(OFILES)
	$(CC) -o sr $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

sr_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o sr -.c $(LDFLAGS) sr.o $(EMLIB)/em_data.a

install:	all
	../install sr

cmp:	all
	-../compare sr

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f sr *.o Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
sr.o:	../share/alloc.h
sr.o:	../share/aux.h
sr.o:	../share/debug.h
sr.o:	../share/files.h
sr.o:	../share/get.h
sr.o:	../share/global.h
sr.o:	../share/go.h
sr.o:	../share/lset.h
sr.o:	../share/map.h
sr.o:	../share/put.h
sr.o:	../share/types.h
sr.o:	sr.h
sr.o:	sr_aux.h
sr.o:	sr_iv.h
sr_aux.o:	../share/aux.h
sr_aux.o:	../share/debug.h
sr_aux.o:	../share/global.h
sr_aux.o:	../share/lset.h
sr_aux.o:	../share/types.h
sr_aux.o:	sr.h
sr_aux.o:	sr_aux.h
sr_aux.o:	sr_xform.h
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
sr_expr.o:	../share/aux.h
sr_expr.o:	../share/debug.h
sr_expr.o:	../share/global.h
sr_expr.o:	../share/lset.h
sr_expr.o:	../share/types.h
sr_expr.o:	sr.h
sr_expr.o:	sr_aux.h
sr_expr.o:	sr_iv.h
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
sr_reduce.o:	../share/alloc.h
sr_reduce.o:	../share/aux.h
sr_reduce.o:	../share/debug.h
sr_reduce.o:	../share/def.h
sr_reduce.o:	../share/global.h
sr_reduce.o:	../share/lset.h
sr_reduce.o:	../share/types.h
sr_reduce.o:	sr.h
sr_reduce.o:	sr_aux.h
sr_reduce.o:	sr_expr.h
sr_reduce.o:	sr_reduce.h
sr_reduce.o:	sr_xform.h
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
