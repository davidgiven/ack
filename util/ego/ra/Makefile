
EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
ra.c ra_items.c ra_lifet.c ra_allocl.c ra_profits.c \
ra_interv.c ra_pack.c ra_xform.c ra_aux.c

OFILES=\
ra.o ra_items.o ra_lifet.o ra_allocl.o ra_profits.o \
ra_interv.o ra_pack.o ra_xform.o ra_aux.o

HFILES=\
ra.h ra_items.h ra_lifet.h ra_allocl.h ra_profits.h \
ra_interv.h ra_pack.h ra_xform.h ra_aux.h

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/aux.o $(SHR)/get.o $(SHR)/put.o $(SHR)/alloc.o $(SHR)/global.o \
$(SHR)/debug.o $(SHR)/files.o $(SHR)/map.o $(SHR)/lset.o $(SHR)/cset.o \
$(SHR)/go.o

SHARE_MFILES=\
$(SHR)/aux.m $(SHR)/get.m $(SHR)/put.m $(SHR)/alloc.m $(SHR)/global.m \
$(SHR)/debug.m $(SHR)/files.m $(SHR)/map.m $(SHR)/lset.m $(SHR)/cset.m \
$(SHR)/go.m

all:	ra

ra:	$(OFILES)
	$(CC) -o ra $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

ra_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o ra -.c $(LDFLAGS) ra.o $(EMLIB)/em_data.a

itemtab.h: itemtab.src makeitems $(EMH)/em_mnem.h
	makeitems $(EMH)/em_mnem.h itemtab.src > itemtab.h

makeitems: makeitems.c
	$(CC) -o makeitems makeitems.c

install:	all
	../install ra

cmp:	all
	-../compare ra

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f ra makeitems itemtab.h *.o Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO

ra.o:	../../../h/em_reg.h
ra.o:	../share/alloc.h
ra.o:	../share/debug.h
ra.o:	../share/files.h
ra.o:	../share/get.h
ra.o:	../share/global.h
ra.o:	../share/go.h
ra.o:	../share/lset.h
ra.o:	../share/map.h
ra.o:	../share/put.h
ra.o:	../share/types.h
ra.o:	ra.h
ra.o:	ra_allocl.h
ra.o:	ra_items.h
ra.o:	ra_pack.h
ra.o:	ra_profits.h
ra.o:	ra_xform.h
ra_allocl.o:	../../../h/em_mnem.h
ra_allocl.o:	../../../h/em_pseu.h
ra_allocl.o:	../../../h/em_reg.h
ra_allocl.o:	../../../h/em_spec.h
ra_allocl.o:	../share/alloc.h
ra_allocl.o:	../share/aux.h
ra_allocl.o:	../share/cset.h
ra_allocl.o:	../share/debug.h
ra_allocl.o:	../share/def.h
ra_allocl.o:	../share/global.h
ra_allocl.o:	../share/lset.h
ra_allocl.o:	../share/map.h
ra_allocl.o:	../share/types.h
ra_allocl.o:	ra.h
ra_allocl.o:	ra_allocl.h
ra_allocl.o:	ra_aux.h
ra_allocl.o:	ra_interv.h
ra_allocl.o:	ra_items.h
ra_aux.o:	../../../h/em_mnem.h
ra_aux.o:	../../../h/em_pseu.h
ra_aux.o:	../../../h/em_reg.h
ra_aux.o:	../../../h/em_spec.h
ra_aux.o:	../share/alloc.h
ra_aux.o:	../share/debug.h
ra_aux.o:	../share/def.h
ra_aux.o:	../share/global.h
ra_aux.o:	../share/lset.h
ra_aux.o:	../share/types.h
ra_aux.o:	ra.h
ra_aux.o:	ra_aux.h
ra_interv.o:	../share/alloc.h
ra_interv.o:	../share/debug.h
ra_interv.o:	../share/global.h
ra_interv.o:	../share/lset.h
ra_interv.o:	../share/types.h
ra_interv.o:	../../../h/em_reg.h
ra_interv.o:	ra.h
ra_interv.o:	ra_interv.h
ra_items.o:	../../../h/em_mnem.h
ra_items.o:	../../../h/em_pseu.h
ra_items.o:	../../../h/em_reg.h
ra_items.o:	../../../h/em_spec.h
ra_items.o:	../share/alloc.h
ra_items.o:	../share/aux.h
ra_items.o:	../share/debug.h
ra_items.o:	../share/def.h
ra_items.o:	../share/global.h
ra_items.o:	../share/lset.h
ra_items.o:	../share/types.h
ra_items.o:	itemtab.h
ra_items.o:	ra.h
ra_items.o:	ra_aux.h
ra_items.o:	ra_items.h
ra_lifet.o:	../../../h/em_mnem.h
ra_lifet.o:	../../../h/em_pseu.h
ra_lifet.o:	../../../h/em_reg.h
ra_lifet.o:	../../../h/em_spec.h
ra_lifet.o:	../share/alloc.h
ra_lifet.o:	../share/aux.h
ra_lifet.o:	../share/debug.h
ra_lifet.o:	../share/def.h
ra_lifet.o:	../share/global.h
ra_lifet.o:	../share/lset.h
ra_lifet.o:	../share/types.h
ra_lifet.o:	ra.h
ra_lifet.o:	ra_aux.h
ra_lifet.o:	ra_items.h
ra_lifet.o:	ra_lifet.h
ra_pack.o:	../../../h/em_reg.h
ra_pack.o:	../share/alloc.h
ra_pack.o:	../share/aux.h
ra_pack.o:	../share/cset.h
ra_pack.o:	../share/debug.h
ra_pack.o:	../share/def.h
ra_pack.o:	../share/global.h
ra_pack.o:	../share/lset.h
ra_pack.o:	../share/types.h
ra_pack.o:	ra.h
ra_pack.o:	ra_aux.h
ra_pack.o:	ra_interv.h
ra_profits.o:	../../../h/em_reg.h
ra_profits.o:	../share/debug.h
ra_profits.o:	../share/global.h
ra_profits.o:	../share/lset.h
ra_profits.o:	../share/types.h
ra_profits.o:	ra.h
ra_profits.o:	ra_aux.h
ra_profits.o:	ra_profits.h
ra_xform.o:	../../../h/em_mes.h
ra_xform.o:	../../../h/em_mnem.h
ra_xform.o:	../../../h/em_pseu.h
ra_xform.o:	../../../h/em_reg.h
ra_xform.o:	../../../h/em_spec.h
ra_xform.o:	../share/alloc.h
ra_xform.o:	../share/aux.h
ra_xform.o:	../share/debug.h
ra_xform.o:	../share/def.h
ra_xform.o:	../share/global.h
ra_xform.o:	../share/lset.h
ra_xform.o:	../share/types.h
ra_xform.o:	ra.h
ra_xform.o:	ra_interv.h
ra_xform.o:	ra_items.h
ra_xform.o:	ra_xform.h
