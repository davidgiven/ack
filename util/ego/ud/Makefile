
EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
ud.c ud_defs.c ud_const.c ud_copy.c ud_aux.c

OFILES=\
ud.o ud_const.o ud_copy.o ud_aux.o ud_defs.o

HFILES=\
ud.h ud_defs.h ud_const.h ud_copy.h ud_aux.h

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/get.o $(SHR)/put.o $(SHR)/map.o $(SHR)/alloc.o $(SHR)/global.o \
$(SHR)/debug.o $(SHR)/lset.o $(SHR)/cset.o $(SHR)/files.o $(SHR)/aux.o \
$(SHR)/locals.o $(SHR)/init_glob.o $(SHR)/go.o

SHARE_MFILES=\
$(SHR)/get.m $(SHR)/put.m $(SHR)/map.m $(SHR)/alloc.m $(SHR)/global.m \
$(SHR)/debug.m $(SHR)/lset.m $(SHR)/cset.m $(SHR)/files.m $(SHR)/aux.m \
$(SHR)/locals.m $(SHR)/init_glob.m $(SHR)/go.m

all:	ud

ud:	$(OFILES)
	$(CC) -o ud $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

ud_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o ud -.c $(LDFLAGS) ud.o $(EMLIB)/em_data.a

install:	all
	../install ud

cmp:	all
	-../compare ud

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f ud *.o Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO

ud.o:	../../../h/em_pseu.h
ud.o:	../../../h/em_spec.h
ud.o:	../share/alloc.h
ud.o:	../share/aux.h
ud.o:	../share/cset.h
ud.o:	../share/debug.h
ud.o:	../share/def.h
ud.o:	../share/files.h
ud.o:	../share/get.h
ud.o:	../share/global.h
ud.o:	../share/locals.h
ud.o:	../share/lset.h
ud.o:	../share/map.h
ud.o:	../share/put.h
ud.o:	../share/types.h
ud.o:	ud.h
ud.o:	ud_const.h
ud.o:	ud_copy.h
ud.o:	ud_defs.h
ud_aux.o:	../../../h/em_mnem.h
ud_aux.o:	../../../h/em_pseu.h
ud_aux.o:	../../../h/em_spec.h
ud_aux.o:	../share/alloc.h
ud_aux.o:	../share/cset.h
ud_aux.o:	../share/debug.h
ud_aux.o:	../share/def.h
ud_aux.o:	../share/global.h
ud_aux.o:	../share/locals.h
ud_aux.o:	../share/lset.h
ud_aux.o:	../share/types.h
ud_aux.o:	../ud/ud.h
ud_aux.o:	../ud/ud_defs.h
ud_const.o:	../../../h/em_mnem.h
ud_const.o:	../../../h/em_pseu.h
ud_const.o:	../../../h/em_spec.h
ud_const.o:	../share/alloc.h
ud_const.o:	../share/aux.h
ud_const.o:	../share/cset.h
ud_const.o:	../share/debug.h
ud_const.o:	../share/def.h
ud_const.o:	../share/global.h
ud_const.o:	../share/locals.h
ud_const.o:	../share/lset.h
ud_const.o:	../share/types.h
ud_const.o:	../ud/ud.h
ud_const.o:	../ud/ud_defs.h
ud_const.o:	ud_aux.h
ud_const.o:	ud_const.h
ud_copy.o:	../../../h/em_mnem.h
ud_copy.o:	../../../h/em_pseu.h
ud_copy.o:	../../../h/em_spec.h
ud_copy.o:	../share/alloc.h
ud_copy.o:	../share/aux.h
ud_copy.o:	../share/cset.h
ud_copy.o:	../share/debug.h
ud_copy.o:	../share/def.h
ud_copy.o:	../share/global.h
ud_copy.o:	../share/locals.h
ud_copy.o:	../share/lset.h
ud_copy.o:	../share/types.h
ud_copy.o:	../ud/ud.h
ud_copy.o:	../ud/ud_defs.h
ud_copy.o:	ud_aux.h
ud_copy.o:	ud_copy.h
ud_defs.o:	../../../h/em_mnem.h
ud_defs.o:	../share/alloc.h
ud_defs.o:	../share/aux.h
ud_defs.o:	../share/cset.h
ud_defs.o:	../share/debug.h
ud_defs.o:	../share/global.h
ud_defs.o:	../share/locals.h
ud_defs.o:	../share/lset.h
ud_defs.o:	../share/map.h
ud_defs.o:	../share/types.h
ud_defs.o:	ud.h
ud_defs.o:	ud_defs.h
