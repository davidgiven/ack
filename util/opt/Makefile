# $Header$

EMHOME=../..
CFILES=main.c getline.c lookup.c var.c process.c backward.c util.c \
	alloc.c putline.c cleanup.c peephole.c flow.c reg.c tes.c \
	pop_push.c

OFILES=main.o getline.o lookup.o var.o process.o backward.o util.o\
	alloc.o putline.o cleanup.o peephole.o flow.o tes.o pop_push.o
ONOGLOB=regnoglob.o
OGLOB=regglob.o
LIBS=$(EMHOME)/lib/em_data.a
CFLAGS=-O -DNDEBUG -I$(EMHOME)/h
LDFLAGS=
LINT=lint
OPR=opr
PROPTS=
CPP=$(EMHOME)/lib/cpp
# LEXLIB is implementation dependent, try -ll or -lln first
LEXLIB=-ll

all:	opt opt2

opt:	$(OFILES) $(ONOGLOB) pattern.o $(LIBS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OFILES) $(ONOGLOB) pattern.o $(LIBS) -o opt

opt2:	$(OFILES) $(OGLOB) pattern.o $(LIBS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OFILES) $(OGLOB) pattern.o $(LIBS) -o opt2

test:	opt testopt
	testopt

cmp:	all
	-cmp opt $(EMHOME)/lib/em_opt
	-cmp opt2 $(EMHOME)/lib/em_opt2
	-cmp em_opt.6 $(EMHOME)/man/em_opt.6

install:all
	-size opt $(EMHOME)/lib/em_opt
	rm -f $(EMHOME)/lib/em_opt
	cp opt $(EMHOME)/lib/em_opt
	-size opt2 $(EMHOME)/lib/em_opt2
	rm -f $(EMHOME)/lib/em_opt2
	cp opt2 $(EMHOME)/lib/em_opt2
	rm -f $(EMHOME)/man/em_opt.6
	cp em_opt.6 $(EMHOME)/man/em_opt.6

pattern.c:	patterns mktab
	$(CPP) patterns | mktab > pattern.c

mktab:	mktab.o $(LIBS)
	$(CC) $(CFLAGS) mktab.o $(LIBS) $(LEXLIB) -o mktab

mktab.o:	scan.c optim.h param.h pattern.h types.h

pop_push.c pop_push.h: $(EMHOME)/etc/em_table pop_push.awk
	awk -f pop_push.awk < $(EMHOME)/etc/em_table > pop_push.c

depend: pattern.c
	sed '/^#AUTOAUTO/,$$d' Makefile >Makefile.new
	echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
	cp reg.c regglob.c
	cp reg.c regnoglob.c
	mkdep $(CFILES) pattern.c regglob.c regnoglob.c | sed 's/\.c:/.o:/' >> Makefile.new
	rm -f regglob.c regnoglob.c
	mv Makefile Makefile.old
	mv Makefile.new Makefile

lint:	$(CFILES) pattern.c
	$(LINT) $(CFLAGS) $(CFILES) pattern.c>lint 2>&1

printall:
	-pr $(PROPTS) Makefile -n *.h `ls $(CFILES)` mktab.y scan.l patterns|$(OPR)
	touch print

print:	Makefile *.h $(CFILES) mktab.y scan.l patterns
	-pr $(PROPTS) -n $? | $(OPR)
	touch print

opr:
	make pr | $(OPR)

pr:
	@pr $(PROPTS) -n Makefile *.h $(CFILES) mktab.y scan.l patterns

clean:
	rm -f *.o opt mktab mktab.c scan.c pattern.c opt2 Out \
		pop_push.c pop_push.h

regnoglob.o:	reg.c
		$(CC) $(CFLAGS) -c reg.c
		mv reg.o regnoglob.o

regglob.o:	reg.c
		$(CC) $(CFLAGS) -c -DGLOBAL_OPT reg.c
		mv reg.o regglob.o

# the next lines are generated automatically
#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
main.o: alloc.h
main.o: ext.h
main.o: param.h
main.o: tes.h
main.o: types.h
getline.o: alloc.h
getline.o: ext.h
getline.o: line.h
getline.o: lookup.h
getline.o: param.h
getline.o: proinf.h
getline.o: tes.h
getline.o: types.h
lookup.o: alloc.h
lookup.o: lookup.h
lookup.o: param.h
lookup.o: proinf.h
lookup.o: tes.h
lookup.o: types.h
var.o: lookup.h
var.o: param.h
var.o: proinf.h
var.o: tes.h
var.o: types.h
process.o: alloc.h
process.o: assert.h
process.o: ext.h
process.o: line.h
process.o: lookup.h
process.o: param.h
process.o: proinf.h
process.o: tes.h
process.o: types.h
backward.o: alloc.h
backward.o: assert.h
backward.o: ext.h
backward.o: line.h
backward.o: lookup.h
backward.o: param.h
backward.o: proinf.h
backward.o: tes.h
backward.o: types.h
util.o: assert.h
util.o: ext.h
util.o: lookup.h
util.o: optim.h
util.o: param.h
util.o: proinf.h
util.o: tes.h
util.o: types.h
alloc.o: alloc.h
alloc.o: assert.h
alloc.o: line.h
alloc.o: lookup.h
alloc.o: param.h
alloc.o: proinf.h
alloc.o: tes.h
alloc.o: types.h
putline.o: alloc.h
putline.o: assert.h
putline.o: ext.h
putline.o: line.h
putline.o: lookup.h
putline.o: optim.h
putline.o: param.h
putline.o: proinf.h
putline.o: tes.h
putline.o: types.h
cleanup.o: assert.h
cleanup.o: ext.h
cleanup.o: lookup.h
cleanup.o: param.h
cleanup.o: types.h
peephole.o: alloc.h
peephole.o: assert.h
peephole.o: ext.h
peephole.o: line.h
peephole.o: lookup.h
peephole.o: optim.h
peephole.o: param.h
peephole.o: pattern.h
peephole.o: proinf.h
peephole.o: tes.h
peephole.o: types.h
flow.o: alloc.h
flow.o: ext.h
flow.o: line.h
flow.o: optim.h
flow.o: param.h
flow.o: proinf.h
flow.o: tes.h
flow.o: types.h
reg.o: alloc.h
reg.o: assert.h
reg.o: ext.h
reg.o: line.h
reg.o: param.h
reg.o: proinf.h
reg.o: tes.h
reg.o: types.h
tes.o: alloc.h
tes.o: assert.h
tes.o: ext.h
tes.o: line.h
tes.o: param.h
tes.o: pop_push.h
tes.o: proinf.h
tes.o: tes.h
tes.o: types.h
pop_push.o: pop_push.h
pattern.o: param.h
pattern.o: pattern.h
pattern.o: types.h
regglob.o: alloc.h
regglob.o: assert.h
regglob.o: ext.h
regglob.o: line.h
regglob.o: param.h
regglob.o: proinf.h
regglob.o: tes.h
regglob.o: types.h
regnoglob.o: alloc.h
regnoglob.o: assert.h
regnoglob.o: ext.h
regnoglob.o: line.h
regnoglob.o: param.h
regnoglob.o: proinf.h
regnoglob.o: tes.h
regnoglob.o: types.h
