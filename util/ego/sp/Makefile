
EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
sp.c

OFILES=\
sp.o

HFILES=

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/get.o $(SHR)/put.o $(SHR)/alloc.o $(SHR)/global.o $(SHR)/debug.o \
$(SHR)/files.o $(SHR)/map.o $(SHR)/lset.o $(SHR)/cset.o $(SHR)/aux.o \
$(SHR)/stack_chg.o $(SHR)/go.o

SHARE_MFILES=\
$(SHR)/get.m $(SHR)/put.m $(SHR)/alloc.m $(SHR)/global.m $(SHR)/debug.m \
$(SHR)/files.m $(SHR)/map.m $(SHR)/lset.m $(SHR)/cset.m $(SHR)/aux.m \
$(SHR)/stack_chg.m $(SHR)/go.m

all:	sp

sp:	$(OFILES)
	$(CC) -o sp $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

sp_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o sp -.c $(LDFLAGS) sp.o $(EMLIB)/em_data.a

install:	all
	../install sp

cmp:	all
	-../compare sp

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f sp *.o Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

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
