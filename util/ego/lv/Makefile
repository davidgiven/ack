EMH=../../../h
EMLIB=../../../lib
SHR=../share

CFILES=\
lv.c

OFILES=\
lv.o

HFILES=\
lv.h

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/get.o $(SHR)/aux.o $(SHR)/put.o $(SHR)/map.o $(SHR)/alloc.o \
$(SHR)/global.o $(SHR)/debug.o $(SHR)/lset.o $(SHR)/cset.o $(SHR)/parser.o \
$(SHR)/files.o $(SHR)/locals.o $(SHR)/init_glob.o $(SHR)/go.o

SHARE_MFILES=\
$(SHR)/get.m $(SHR)/aux.m $(SHR)/put.m $(SHR)/map.m $(SHR)/alloc.m \
$(SHR)/global.m $(SHR)/debug.m $(SHR)/lset.m $(SHR)/cset.m $(SHR)/parser.m \
$(SHR)/files.m $(SHR)/locals.m $(SHR)/init_glob.m $(SHR)/go.m

lv:	$(OFILES)
	$(CC) -o lv $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

lv_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o lv -.c $(LDFLAGS) lv.o $(EMLIB)/em_data.a

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

pr:	$(PRFILES)
	@pr $?
	@touch pr

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO

lv.o:	../../../h/em_mnem.h
lv.o:	../../../h/em_pseu.h
lv.o:	../../../h/em_spec.h
lv.o:	../share/alloc.h
lv.o:	../share/aux.h
lv.o:	../share/cset.h
lv.o:	../share/debug.h
lv.o:	../share/def.h
lv.o:	../share/files.h
lv.o:	../share/get.h
lv.o:	../share/global.h
lv.o:	../share/go.h
lv.o:	../share/init_glob.h
lv.o:	../share/locals.h
lv.o:	../share/lset.h
lv.o:	../share/map.h
lv.o:	../share/parser.h
lv.o:	../share/put.h
lv.o:	../share/types.h
lv.o:	lv.h
