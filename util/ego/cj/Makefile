
EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
cj.c

OFILES=\
cj.o

HFILES=

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/get.o $(SHR)/put.o $(SHR)/alloc.o $(SHR)/global.o $(SHR)/debug.o \
$(SHR)/files.o $(SHR)/map.o $(SHR)/lset.o $(SHR)/cset.o $(SHR)/aux.o \
$(SHR)/stack_chg.o $(SHR)/go.o

SHARE_MFILES=\
$(SHR)/get.m $(SHR)/put.m $(SHR)/alloc.m $(SHR)/global.m $(SHR)/debug.m \
$(SHR)/files.m $(SHR)/map.m $(SHR)/lset.m $(SHR)/cset.m $(SHR)/aux.m $(SHR)/stack_chg.m $(SHR)/go.m

all:	cj

cj:	$(OFILES)
	$(CC) -o \
	cj $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

cj_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o cj -.c $(LDFLAGS) cj.o $(EMLIB)/em_data.a

install:	all
	../install cj

cmp:	all
	-../compare cj

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f cj *.o Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO

cj.o:	../../../h/em_mnem.h
cj.o:	../../../h/em_spec.h
cj.o:	../share/alloc.h
cj.o:	../share/aux.h
cj.o:	../share/debug.h
cj.o:	../share/def.h
cj.o:	../share/files.h
cj.o:	../share/get.h
cj.o:	../share/global.h
cj.o:	../share/go.h
cj.o:	../share/lset.h
cj.o:	../share/map.h
cj.o:	../share/put.h
cj.o:	../share/stack_chg.h
cj.o:	../share/types.h
