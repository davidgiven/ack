EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH)
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
il.c il1_anal.c il1_cal.c il1_formal.c il1_aux.c il2_aux.c \
il3_subst.c il3_change.c il3_aux.c il_aux.c

OFILES=\
il.o il1_anal.o il1_cal.o il1_formal.o il1_aux.o il2_aux.o \
il3_change.o il3_subst.o il3_aux.o il_aux.o 

HFILES=\
il.h il1_anal.h il1_cal.h il1_formal.h il1_aux.h il2_aux.h \
il3_subst.h il3_change.h il3_aux.h il_aux.h

PRFILES=\
$(CFILES) $(HFILES) Makefile

SHARE_OFILES=\
$(SHR)/get.o $(SHR)/put.o $(SHR)/alloc.o $(SHR)/global.o $(SHR)/debug.o \
$(SHR)/files.o $(SHR)/map.o $(SHR)/lset.o $(SHR)/cset.o $(SHR)/parser.o \
$(SHR)/aux.o $(SHR)/go.o

SHARE_MFILES=\
$(SHR)/get.m $(SHR)/put.m $(SHR)/alloc.m $(SHR)/global.m $(SHR)/debug.m \
$(SHR)/files.m $(SHR)/map.m $(SHR)/lset.m $(SHR)/cset.m $(SHR)/parser.m \
$(SHR)/aux.m $(SHR)/go.m

all:	il

il:	$(OFILES)
	$(CC) -o il $(LDFLAGS) $(OFILES) $(SHARE_OFILES) $(EMLIB)/em_data.a

il_ack:	$(CFILES) $(SHARE_MFILES)
	$(CC) -c.o $(CFLAGS) $(CFILES) $(SHARE_MFILES)
	$(CC) -o il -.c $(LDFLAGS) il.o $(EMLIB)/em_data.a

install:	all
	../install il

cmp:	all
	-../compare il

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f il *.o Out out nohup.out

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
il.o:	../share/alloc.h
il.o:	../share/debug.h
il.o:	../share/files.h
il.o:	../share/get.h
il.o:	../share/global.h
il.o:	../share/go.h
il.o:	../share/lset.h
il.o:	../share/map.h
il.o:	../share/put.h
il.o:	../share/types.h
il.o:	il.h
il.o:	il1_anal.h
il.o:	il2_aux.h
il.o:	il3_subst.h
il.o:	il_aux.h
il1_anal.o:	../share/alloc.h
il1_anal.o:	../share/aux.h
il1_anal.o:	../share/debug.h
il1_anal.o:	../share/global.h
il1_anal.o:	../share/lset.h
il1_anal.o:	../share/put.h
il1_anal.o:	../share/types.h
il1_anal.o:	il.h
il1_anal.o:	il1_anal.h
il1_anal.o:	il1_aux.h
il1_anal.o:	il1_cal.h
il1_anal.o:	il1_formal.h
il1_anal.o:	il_aux.h
il1_aux.o:	../share/alloc.h
il1_aux.o:	../share/debug.h
il1_aux.o:	../share/global.h
il1_aux.o:	../share/lset.h
il1_aux.o:	../share/types.h
il1_aux.o:	il.h
il1_aux.o:	il1_aux.h
il1_aux.o:	il_aux.h
il1_cal.o:	../share/alloc.h
il1_cal.o:	../share/debug.h
il1_cal.o:	../share/global.h
il1_cal.o:	../share/lset.h
il1_cal.o:	../share/parser.h
il1_cal.o:	../share/types.h
il1_cal.o:	il.h
il1_cal.o:	il1_aux.h
il1_cal.o:	il1_cal.h
il1_formal.o:	../share/alloc.h
il1_formal.o:	../share/debug.h
il1_formal.o:	../share/global.h
il1_formal.o:	../share/lset.h
il1_formal.o:	../share/types.h
il1_formal.o:	il.h
il1_formal.o:	il1_aux.h
il1_formal.o:	il1_formal.h
il2_aux.o:	../share/alloc.h
il2_aux.o:	../share/aux.h
il2_aux.o:	../share/debug.h
il2_aux.o:	../share/get.h
il2_aux.o:	../share/global.h
il2_aux.o:	../share/lset.h
il2_aux.o:	../share/types.h
il2_aux.o:	il.h
il2_aux.o:	il2_aux.h
il2_aux.o:	il_aux.h
il3_aux.o:	../share/alloc.h
il3_aux.o:	../share/debug.h
il3_aux.o:	../share/global.h
il3_aux.o:	../share/types.h
il3_aux.o:	il.h
il3_aux.o:	il3_aux.h
il3_aux.o:	il_aux.h
il3_change.o:	../share/alloc.h
il3_change.o:	../share/aux.h
il3_change.o:	../share/debug.h
il3_change.o:	../share/def.h
il3_change.o:	../share/get.h
il3_change.o:	../share/global.h
il3_change.o:	../share/lset.h
il3_change.o:	../share/put.h
il3_change.o:	../share/types.h
il3_change.o:	il.h
il3_change.o:	il3_aux.h
il3_change.o:	il3_change.h
il3_change.o:	il_aux.h
il3_subst.o:	../share/alloc.h
il3_subst.o:	../share/debug.h
il3_subst.o:	../share/get.h
il3_subst.o:	../share/global.h
il3_subst.o:	../share/lset.h
il3_subst.o:	../share/types.h
il3_subst.o:	il.h
il3_subst.o:	il3_aux.h
il3_subst.o:	il3_change.h
il3_subst.o:	il3_subst.h
il3_subst.o:	il_aux.h
il_aux.o:	../share/alloc.h
il_aux.o:	../share/debug.h
il_aux.o:	../share/get.h
il_aux.o:	../share/global.h
il_aux.o:	../share/lset.h
il_aux.o:	../share/map.h
il_aux.o:	../share/put.h
il_aux.o:	../share/types.h
il_aux.o:	il.h
il_aux.o:	il_aux.h
