EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share
LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
ic.c ic_aux.c ic_lib.c ic_lookup.c ic_io.c

OFILES=\
ic.o ic_aux.o ic_lookup.o ic_io.o ic_lib.o

HFILES=\
ic.h ic_aux.h ic_lib.h ic_lookup.h ic_io.h

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/put.o $(SHR)/alloc.o $(SHR)/global.o $(SHR)/debug.o \
$(SHR)/files.o $(SHR)/map.o $(SHR)/lset.o $(SHR)/cset.o $(SHR)/aux.o

SHARE_MFILES=\
$(SHR)/put.m $(SHR)/alloc.m $(SHR)/global.m $(SHR)/debug.m \
$(SHR)/files.m $(SHR)/map.m $(SHR)/lset.m $(SHR)/cset.m $(SHR)/aux.m

all:	ic

ic:	$(OFILES)
	$(CC) -o ic $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

ic_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o ic -.c $(LDFLAGS) ic.o $(EMLIB)/em_data.a

install:	all
	../install ic

cmp:	all
	-../compare ic

clean:
	rm -f *.o ic Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
ic.o:	../../../h/em_flag.h
ic.o:	../../../h/em_mes.h
ic.o:	../../../h/em_pseu.h
ic.o:	../../../h/em_spec.h
ic.o:	../share/alloc.h
ic.o:	../share/aux.h
ic.o:	../share/debug.h
ic.o:	../share/def.h
ic.o:	../share/files.h
ic.o:	../share/global.h
ic.o:	../share/map.h
ic.o:	../share/put.h
ic.o:	../share/types.h
ic.o:	ic.h
ic.o:	ic_aux.h
ic.o:	ic_io.h
ic.o:	ic_lib.h
ic.o:	ic_lookup.h
ic_aux.o:	../../../h/em_mnem.h
ic_aux.o:	../../../h/em_pseu.h
ic_aux.o:	../../../h/em_spec.h
ic_aux.o:	../share/alloc.h
ic_aux.o:	../share/aux.h
ic_aux.o:	../share/debug.h
ic_aux.o:	../share/def.h
ic_aux.o:	../share/global.h
ic_aux.o:	../share/types.h
ic_aux.o:	ic.h
ic_aux.o:	ic_aux.h
ic_aux.o:	ic_io.h
ic_aux.o:	ic_lookup.h
ic_io.o:	../../../h/em_pseu.h
ic_io.o:	../../../h/em_spec.h
ic_io.o:	../share/alloc.h
ic_io.o:	../share/debug.h
ic_io.o:	../share/types.h
ic_io.o:	ic.h
ic_io.o:	ic_io.h
ic_io.o:	ic_lookup.h
ic_lib.o:	../../../h/em_mes.h
ic_lib.o:	../../../h/em_pseu.h
ic_lib.o:	../../../h/em_spec.h
ic_lib.o:	../share/debug.h
ic_lib.o:	../share/files.h
ic_lib.o:	../share/global.h
ic_lib.o:	../share/types.h
ic_lib.o:	ic.h
ic_lib.o:	ic_io.h
ic_lib.o:	ic_lib.h
ic_lib.o:	ic_lookup.h
ic_lookup.o:	../../../h/em_spec.h
ic_lookup.o:	../share/alloc.h
ic_lookup.o:	../share/debug.h
ic_lookup.o:	../share/map.h
ic_lookup.o:	../share/types.h
ic_lookup.o:	ic.h
ic_lookup.o:	ic_lookup.h
