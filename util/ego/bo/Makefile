EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH)
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
bo.c

OFILES=\
bo.o

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

all:	bo

bo:	$(OFILES)
	$(CC) -o bo $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

bo_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o bo -.c $(LDFLAGS) bo.o $(EMLIB)/em_data.a

install:	all
	../install bo

cmp:	all
	-../compare bo

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f bo *.o Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
bo.o:	../share/alloc.h
bo.o:	../share/aux.h
bo.o:	../share/debug.h
bo.o:	../share/def.h
bo.o:	../share/files.h
bo.o:	../share/get.h
bo.o:	../share/global.h
bo.o:	../share/go.h
bo.o:	../share/lset.h
bo.o:	../share/map.h
bo.o:	../share/put.h
bo.o:	../share/types.h
