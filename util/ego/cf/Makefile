
EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
cf.c cf_succ.c cf_idom.c cf_loop.c

OFILES=\
cf.o cf_idom.o cf_loop.o cf_succ.o 

HFILES=\
cf.h cf_succ.h cf_idom.h cf_loop.h

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/get.o $(SHR)/put.o $(SHR)/alloc.o $(SHR)/global.o \
$(SHR)/debug.o $(SHR)/files.o $(SHR)/map.o $(SHR)/lset.o \
$(SHR)/cset.o $(SHR)/aux.o

SHARE_MFILES=\
$(SHR)/get.m $(SHR)/put.m $(SHR)/alloc.m $(SHR)/global.m \
$(SHR)/debug.m $(SHR)/files.m $(SHR)/map.m $(SHR)/lset.m \
$(SHR)/cset.m $(SHR)/aux.m

all:	cf

cf:	$(OFILES)
	$(CC) -o cf $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

cf_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o cf -.c $(LDFLAGS) cf.o $(EMLIB)/em_data.a

install:	all
	../install cf

cmp:	all
	-../compare cf

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f cf *.o Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

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
