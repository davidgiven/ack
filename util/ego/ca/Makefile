
EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
ca.c ca_put.c

OFILES=\
ca.o ca_put.o

HFILES=\
ca.h ca_put.h

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/get.o $(SHR)/alloc.o $(SHR)/global.o $(SHR)/aux.o $(SHR)/debug.o \
$(SHR)/lset.o $(SHR)/cset.o $(SHR)/files.o $(SHR)/map.o

SHARE_MFILES=\
$(SHR)/get.m $(SHR)/alloc.m $(SHR)/global.m $(SHR)/aux.m $(SHR)/debug.m \
$(SHR)/lset.m $(SHR)/cset.m $(SHR)/files.m $(SHR)/map.m

all:	ca

ca:	$(OFILES)
	$(CC) -o ca $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

ca_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o ca -.c $(LDFLAGS) ca.o $(EMLIB)/em_data.a

install:	all
	../install ca

cmp:	all
	-../compare ca

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f ca *.o Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO

ca.o:	../share/alloc.h
ca.o:	../share/debug.h
ca.o:	../share/files.h
ca.o:	../share/get.h
ca.o:	../share/global.h
ca.o:	../share/lset.h
ca.o:	../share/map.h
ca.o:	../share/types.h
ca.o:	ca.h
ca.o:	ca_put.h
ca_put.o:	../../../h/em_flag.h
ca_put.o:	../../../h/em_mes.h
ca_put.o:	../../../h/em_mnem.h
ca_put.o:	../../../h/em_pseu.h
ca_put.o:	../../../h/em_spec.h
ca_put.o:	../share/alloc.h
ca_put.o:	../share/debug.h
ca_put.o:	../share/def.h
ca_put.o:	../share/map.h
ca_put.o:	../share/types.h
ca_put.o:	ca.h
