# $Header$

EMHOME=../..
CFILES=main.c getline.c lookup.c var.c process.c backward.c util.c\
	alloc.c putline.c cleanup.c peephole.c flow.c reg.c
OFILES=main.o getline.o lookup.o var.o process.o backward.o util.o\
	alloc.o putline.o cleanup.o peephole.o flow.o
ONOGLOB=regnoglob.o
OGLOB=regglob.o
KFILES=main.k getline.k lookup.k var.k process.k backward.k util.k\
	alloc.k putline.k cleanup.k peephole.k flow.k regnoglob.k regglob.k
LIBS=$(EMHOME)/lib/em_data.a
CFLAGS=-O -DNDEBUG -I$(EMHOME)/h
LDFLAGS=-i
LINT=lint
OPR=wide|opr
XREF=xref -c -w80
PROPTS=
# LEXLIB is implementation dependent, try -ll or -lln first
LEXLIB=-ll

.DEFAULT:
	co -q $<

all:	opt opt2

opt:    $(OFILES) $(ONOGLOB) pattern.o $(LIBS)
	cc $(LDFLAGS) $(CFLAGS) $(OFILES) $(ONOGLOB) pattern.o $(LIBS) -o opt

opt2:   $(OFILES) $(OGLOB) pattern.o $(LIBS)
	cc $(LDFLAGS) $(CFLAGS) $(OFILES) $(OGLOB) pattern.o $(LIBS) -o opt2

test:   opt testopt
	testopt

cmp :	all
	-cmp opt $(EMHOME)/lib/em_opt
	-cmp opt2 $(EMHOME)/lib/em_opt2

install:all
	-size opt $(EMHOME)/lib/em_opt
	rm -f $(EMHOME)/lib/em_opt
	cp opt $(EMHOME)/lib/em_opt
	-size opt2 $(EMHOME)/lib/em_opt2
	rm -f $(EMHOME)/lib/em_opt2
	cp opt2 $(EMHOME)/lib/em_opt2

pattern.c:      patterns mktab
	/lib/cpp patterns | mktab > pattern.c

mktab:  mktab.o $(LIBS)
	cc $(CFLAGS) mktab.o $(LIBS) $(LEXLIB) -o mktab

depend: regglob.c regnoglob.c pattern.c
	sed '/^#AUTOAUTO/,$$d' Makefile >Makefile.new
	echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
	mkdep $(CFILES) pattern.c regglob.c regnoglob.c | sed 's/\.c:/.o:/' >> Makefile.new
	mv Makefile Makefile.old
	mv Makefile.new Makefile

lint:   $(CFILES) pattern.c
	$(LINT) $(CFILES) pattern.c>lint 2>&1

printall:
	-pr $(PROPTS) Makefile -n *.h `ls $(CFILES)` mktab.y scan.l patterns|$(OPR)
	touch print

print:  Makefile *.h $(CFILES) mktab.y scan.l patterns
	-pr $(PROPTS) -n $? | $(OPR)

opr:
	make pr ^ $(OPR)

pr:
	@pr $(PROPTS) -n Makefile *.h $(CFILES) mktab.y scan.l patterns

xref:
	$(XREF) *.h $(CFILES) | pr $(PROPTS) -h "XREF EMOPT"|$(OPR)&

sizes:  opt
	-nm opt | sort -n| /usr/plain/bin/map

clean:
	rm -f *.o opt mktab mktab.c scan.c pattern.c opt2 Out

regnoglob.c:	reg.c
		cp reg.c regnoglob.c

regglob.c:	reg.c
		cp reg.c regglob.c

regglob.o:	regglob.c
		$(CC) $(CFLAGS) -c -DGLOBAL_OPT regglob.c

kfiles: $(KFILES)

.SUFFIXES: .k
.c.k: ; cem -c $*.c

# the next lines are generated automatically
#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
main.o: ../../h/em_spec.h alloc.h ext.h param.h types.h
getline.o: ../../h/em_flag.h ../../h/em_mes.h ../../h/em_pseu.h ../../h/em_spec.h alloc.h ext.h line.h lookup.h param.h proinf.h types.h
lookup.o: alloc.h lookup.h param.h proinf.h types.h
var.o: lookup.h param.h proinf.h types.h
process.o: ../../h/em_pseu.h ../../h/em_spec.h alloc.h assert.h ext.h line.h lookup.h param.h proinf.h types.h
backward.o: ../../h/em_mnem.h ../../h/em_pseu.h ../../h/em_spec.h alloc.h assert.h ext.h line.h lookup.h param.h proinf.h types.h
util.o: assert.h ext.h lookup.h optim.h param.h proinf.h types.h
alloc.o: alloc.h assert.h line.h lookup.h param.h proinf.h types.h
putline.o: ../../h/em_flag.h ../../h/em_mnem.h ../../h/em_pseu.h ../../h/em_spec.h alloc.h assert.h ext.h line.h lookup.h optim.h param.h proinf.h types.h
cleanup.o: ../../h/em_mes.h ../../h/em_pseu.h ../../h/em_spec.h assert.h ext.h lookup.h param.h types.h
peephole.o: ../../h/em_mnem.h ../../h/em_spec.h alloc.h assert.h ext.h line.h lookup.h optim.h param.h pattern.h proinf.h types.h
flow.o: ../../h/em_flag.h ../../h/em_mnem.h ../../h/em_spec.h alloc.h ext.h line.h optim.h param.h proinf.h types.h
reg.o: ../../h/em_mes.h ../../h/em_pseu.h ../../h/em_spec.h alloc.h assert.h ext.h line.h param.h proinf.h types.h
pattern.o: param.h pattern.h types.h
regglob.o: ../../h/em_mes.h ../../h/em_pseu.h ../../h/em_spec.h alloc.h assert.h ext.h line.h param.h proinf.h types.h
regnoglob.o: ../../h/em_mes.h ../../h/em_pseu.h ../../h/em_spec.h alloc.h assert.h ext.h line.h param.h proinf.h types.h
