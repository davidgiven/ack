
EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
cs.c cs_alloc.c cs_aux.c cs_avail.c cs_debug.c cs_elim.c \
cs_entity.c cs_kill.c cs_partit.c cs_profit.c cs_getent.c \
cs_stack.c cs_vnm.c

OFILES=\
cs.o cs_alloc.o cs_aux.o cs_avail.o cs_debug.o cs_elim.o \
cs_entity.o cs_kill.o cs_partit.o cs_profit.o cs_getent.o \
cs_stack.o cs_vnm.o

HFILES=\
cs.h cs_alloc.h cs_aux.h cs_avail.h cs_debug.h cs_elim.h \
cs_entity.h cs_kill.h cs_partit.h cs_profit.h cs_getent.h \
cs_stack.h cs_vnm.h

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/get.o $(SHR)/put.o $(SHR)/alloc.o $(SHR)/global.o $(SHR)/debug.o\
$(SHR)/files.o $(SHR)/map.o $(SHR)/lset.o $(SHR)/cset.o $(SHR)/aux.o\
$(SHR)/go.o

SHARE_MFILES=\
$(SHR)/get.m $(SHR)/put.m $(SHR)/alloc.m $(SHR)/global.m $(SHR)/debug.m\
$(SHR)/files.m $(SHR)/map.m $(SHR)/lset.m $(SHR)/cset.m $(SHR)/aux.m\
$(SHR)/go.m

all:	cs

cs:	$(OFILES)
	$(CC) -o cs $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

cs_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o cs -.c $(LDFLAGS) cs.o $(EMLIB)/em_data.a

install:	all
	../install cs

cmp:	all
	-../compare cs

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f cs *.o Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
cs.o:	../share/debug.h
cs.o:	../share/go.h
cs.o:	../share/types.h
cs.o:	cs.h
cs.o:	cs_aux.h
cs.o:	cs_avail.h
cs.o:	cs_debug.h
cs.o:	cs_elim.h
cs.o:	cs_entity.h
cs.o:	cs_profit.h
cs.o:	cs_stack.h
cs.o:	cs_vnm.h
cs_alloc.o:	../share/alloc.h
cs_alloc.o:	../share/types.h
cs_alloc.o:	cs.h
cs_aux.o:	../share/aux.h
cs_aux.o:	../share/debug.h
cs_aux.o:	../share/global.h
cs_aux.o:	../share/lset.h
cs_aux.o:	../share/types.h
cs_aux.o:	cs.h
cs_aux.o:	cs_entity.h
cs_avail.o:	../../../h/em_mnem.h
cs_avail.o:	../share/aux.h
cs_avail.o:	../share/debug.h
cs_avail.o:	../share/global.h
cs_avail.o:	../share/lset.h
cs_avail.o:	../share/types.h
cs_avail.o:	cs.h
cs_avail.o:	cs_alloc.h
cs_avail.o:	cs_aux.h
cs_avail.o:	cs_getent.h
cs_debug.o:	../../../h/em_spec.h
cs_debug.o:	../share/debug.h
cs_debug.o:	../share/lset.h
cs_debug.o:	../share/types.h
cs_debug.o:	cs.h
cs_debug.o:	cs_aux.h
cs_debug.o:	cs_avail.h
cs_debug.o:	cs_entity.h
cs_elim.o:	../../../h/em_mnem.h
cs_elim.o:	../../../h/em_reg.h
cs_elim.o:	../share/alloc.h
cs_elim.o:	../share/aux.h
cs_elim.o:	../share/debug.h
cs_elim.o:	../share/global.h
cs_elim.o:	../share/lset.h
cs_elim.o:	../share/types.h
cs_elim.o:	cs.h
cs_elim.o:	cs_alloc.h
cs_elim.o:	cs_aux.h
cs_elim.o:	cs_avail.h
cs_elim.o:	cs_debug.h
cs_elim.o:	cs_partit.h
cs_elim.o:	cs_profit.h
cs_entity.o:	../share/debug.h
cs_entity.o:	../share/global.h
cs_entity.o:	../share/lset.h
cs_entity.o:	../share/types.h
cs_entity.o:	cs.h
cs_entity.o:	cs_aux.h
cs_getent.o:	../../../h/em_mnem.h
cs_getent.o:	../share/aux.h
cs_getent.o:	../share/debug.h
cs_getent.o:	../share/global.h
cs_getent.o:	../share/types.h
cs_getent.o:	cs.h
cs_getent.o:	cs_aux.h
cs_getent.o:	cs_entity.h
cs_getent.o:	cs_stack.h
cs_kill.o:	../../../h/em_mnem.h
cs_kill.o:	../share/aux.h
cs_kill.o:	../share/cset.h
cs_kill.o:	../share/debug.h
cs_kill.o:	../share/global.h
cs_kill.o:	../share/lset.h
cs_kill.o:	../share/types.h
cs_kill.o:	cs.h
cs_kill.o:	cs_aux.h
cs_kill.o:	cs_avail.h
cs_kill.o:	cs_debug.h
cs_kill.o:	cs_entity.h
cs_partit.o:	../../../h/em_mnem.h
cs_partit.o:	../../../h/em_pseu.h
cs_partit.o:	../../../h/em_reg.h
cs_partit.o:	../../../h/em_spec.h
cs_partit.o:	../share/aux.h
cs_partit.o:	../share/debug.h
cs_partit.o:	../share/global.h
cs_partit.o:	../share/types.h
cs_partit.o:	cs.h
cs_partit.o:	cs_stack.h
cs_profit.o:	../../../h/em_mnem.h
cs_profit.o:	../../../h/em_spec.h
cs_profit.o:	../share/aux.h
cs_profit.o:	../share/cset.h
cs_profit.o:	../share/debug.h
cs_profit.o:	../share/global.h
cs_profit.o:	../share/lset.h
cs_profit.o:	../share/types.h
cs_profit.o:	cs.h
cs_profit.o:	cs_aux.h
cs_profit.o:	cs_avail.h
cs_profit.o:	cs_partit.h
cs_stack.o:	../share/aux.h
cs_stack.o:	../share/debug.h
cs_stack.o:	../share/global.h
cs_stack.o:	../share/types.h
cs_stack.o:	cs.h
cs_stack.o:	cs_aux.h
cs_valno.o:	../share/debug.h
cs_valno.o:	../share/types.h
cs_valno.o:	cs.h
cs_vnm.o:	../../../h/em_mnem.h
cs_vnm.o:	../share/aux.h
cs_vnm.o:	../share/debug.h
cs_vnm.o:	../share/global.h
cs_vnm.o:	../share/types.h
cs_vnm.o:	cs.h
cs_vnm.o:	cs_alloc.h
cs_vnm.o:	cs_aux.h
cs_vnm.o:	cs_avail.h
cs_vnm.o:	cs_entity.h
cs_vnm.o:	cs_getent.h
cs_vnm.o:	cs_kill.h
cs_vnm.o:	cs_partit.h
cs_vnm.o:	cs_stack.h
