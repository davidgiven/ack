# $Header$

CFILES=main.c getline.c lookup.c var.c process.c backward.c util.c\
	alloc.c putline.c cleanup.c peephole.c flow.c reg.c
OFILES=main.o getline.o lookup.o var.o process.o backward.o util.o\
	alloc.o putline.o cleanup.o peephole.o flow.o reg.o
KFILES=main.k getline.k lookup.k var.k process.k backward.k util.k\
	alloc.k putline.k cleanup.k peephole.k flow.k reg.k
LIBS=../../lib/em_data.a
CFLAGS=-O -DNDEBUG
LDFLAGS=-i
LINT=lint
OPR=wide|opr
XREF=xref -c -w80
PROPTS=
# LEXLIB is implementation dependent, try -ll or -lln first
LEXLIB=-ll

.DEFAULT:
	co -q $<

opt:    $(OFILES) pattern.o $(LIBS)
	cc $(LDFLAGS) $(CFLAGS) $(OFILES) pattern.o $(LIBS) -o opt

test:   opt testopt
	testopt

cmp :   opt
	cmp opt ../../lib/em_opt

install:opt
	size opt ../../lib/em_opt
	cp opt ../../lib/em_opt

pattern.c:      patterns mktab
	/lib/cpp patterns | mktab > pattern.c

mktab:  mktab.o $(LIBS)
	cc $(CFLAGS) mktab.o $(LIBS) $(LEXLIB) -o mktab

depend: makedepend
	makedepend

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
	rm -f *.o opt mktab mktab.c scan.c pattern.c

kfiles: $(KFILES)

.SUFFIXES: .k
.c.k: ; cem -c $*.c

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
alloc.o:	alloc.h
alloc.o:	assert.h
alloc.o:	line.h
alloc.o:	lookup.h
alloc.o:	param.h
alloc.o:	proinf.h
alloc.o:	types.h
backward.o:	../../h/em_mnem.h
backward.o:	../../h/em_pseu.h
backward.o:	../../h/em_spec.h
backward.o:	alloc.h
backward.o:	assert.h
backward.o:	ext.h
backward.o:	line.h
backward.o:	lookup.h
backward.o:	param.h
backward.o:	proinf.h
backward.o:	types.h
cleanup.o:	../../h/em_mes.h
cleanup.o:	../../h/em_pseu.h
cleanup.o:	../../h/em_spec.h
cleanup.o:	assert.h
cleanup.o:	ext.h
cleanup.o:	lookup.h
cleanup.o:	param.h
cleanup.o:	types.h
flow.o:	../../h/em_flag.h
flow.o:	../../h/em_mnem.h
flow.o:	../../h/em_spec.h
flow.o:	alloc.h
flow.o:	ext.h
flow.o:	line.h
flow.o:	optim.h
flow.o:	param.h
flow.o:	proinf.h
flow.o:	types.h
getline.o:	../../h/em_flag.h
getline.o:	../../h/em_mes.h
getline.o:	../../h/em_pseu.h
getline.o:	../../h/em_spec.h
getline.o:	alloc.h
getline.o:	assert.h
getline.o:	ext.h
getline.o:	line.h
getline.o:	lookup.h
getline.o:	param.h
getline.o:	proinf.h
getline.o:	types.h
lookup.o:	alloc.h
lookup.o:	lookup.h
lookup.o:	param.h
lookup.o:	proinf.h
lookup.o:	types.h
main.o:	../../h/em_spec.h
main.o:	alloc.h
main.o:	ext.h
main.o:	param.h
main.o:	types.h
mktab.o:	../../h/em_mnem.h
mktab.o:	../../h/em_spec.h
mktab.o:	optim.h
mktab.o:	param.h
mktab.o:	pattern.h
mktab.o:	scan.c
mktab.o:	types.h
pattern.o:	param.h
pattern.o:	pattern.h
pattern.o:	types.h
peephole.o:	../../h/em_mnem.h
peephole.o:	../../h/em_spec.h
peephole.o:	alloc.h
peephole.o:	assert.h
peephole.o:	ext.h
peephole.o:	line.h
peephole.o:	lookup.h
peephole.o:	optim.h
peephole.o:	param.h
peephole.o:	pattern.h
peephole.o:	proinf.h
peephole.o:	types.h
process.o:	../../h/em_pseu.h
process.o:	../../h/em_spec.h
process.o:	alloc.h
process.o:	assert.h
process.o:	ext.h
process.o:	line.h
process.o:	lookup.h
process.o:	param.h
process.o:	proinf.h
process.o:	types.h
putline.o:	../../h/em_flag.h
putline.o:	../../h/em_mnem.h
putline.o:	../../h/em_pseu.h
putline.o:	../../h/em_spec.h
putline.o:	alloc.h
putline.o:	assert.h
putline.o:	ext.h
putline.o:	line.h
putline.o:	lookup.h
putline.o:	optim.h
putline.o:	param.h
putline.o:	proinf.h
putline.o:	types.h
reg.o:	../../h/em_mes.h
reg.o:	../../h/em_pseu.h
reg.o:	../../h/em_spec.h
reg.o:	alloc.h
reg.o:	assert.h
reg.o:	ext.h
reg.o:	line.h
reg.o:	param.h
reg.o:	proinf.h
reg.o:	types.h
scan.o:	stdio.h
special.o:	param.h
special.o:	types.h
util.o:	assert.h
util.o:	ext.h
util.o:	lookup.h
util.o:	optim.h
util.o:	param.h
util.o:	proinf.h
util.o:	types.h
var.o:	lookup.h
var.o:	param.h
var.o:	proinf.h
var.o:	types.h
