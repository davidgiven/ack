PREFLAGS=-I../../../h -DNDEBUG
PFLAGS=
CFLAGS=$(PREFLAGS) $(PFLAGS) -O
LDFLAGS=-i $(PFLAGS)
LINTOPTS=-hbxac
LIBS=../../../lib/em_data.a
CFILES=codegen.c compute.c equiv.c fillem.c gencode.c glosym.c main.c\
       move.c nextem.c reg.c regvar.c salloc.c state.c subr.c var.c
OFILES=codegen.o compute.o equiv.o fillem.o gencode.o glosym.o main.o\
       move.o nextem.o reg.o regvar.o salloc.o state.o subr.o var.o

cg: tables.o $(OFILES)
	cc $(LDFLAGS) $(OFILES) tables.o $(LIBS) -o cg
	touch tables.h tables.c tables.o $(OFILES) cg

tables.o: tables.c
	cc -c $(PREFLAGS) tables.c

install: cg
	cp cg ../../../lib/pdp_cg

cmp:	 cg
	-../compare cg


tables.h tables.c: pdptable
	-mv tables.h tables.h.save
	../../../lib/cpp -P pdptable | ../../../lib/cgg > debug.out
	-if cmp -s tables.h.save tables.h; then mv tables.h.save tables.h; else exit 0; fi
	-if cmp -s /dev/null tables.h; then mv tables.h.save tables.h; else exit 0; fi

lint: $(CFILES)
	lint $(LINTOPTS) $(PREFLAGS) $(CFILES)
clean:
	rm -f *.o tables.c
depend:
	makedepend `grep -v '\.h' sources`
# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
codegen.o:	assert.h
codegen.o:	data.h
codegen.o:	equiv.h
codegen.o:	extern.h
codegen.o:	param.h
codegen.o:	result.h
codegen.o:	state.h
codegen.o:	tables.h
codegen.o:	types.h
compute.o:	assert.h
compute.o:	data.h
compute.o:	extern.h
compute.o:	glosym.h
compute.o:	param.h
compute.o:	result.h
compute.o:	tables.h
compute.o:	types.h
equiv.o:	assert.h
equiv.o:	data.h
equiv.o:	equiv.h
equiv.o:	extern.h
equiv.o:	param.h
equiv.o:	result.h
equiv.o:	tables.h
equiv.o:	types.h
fillem.o:	assert.h
fillem.o:	data.h
fillem.o:	extern.h
fillem.o:	mach.c
fillem.o:	mach.h
fillem.o:	param.h
fillem.o:	regvar.h
fillem.o:	result.h
fillem.o:	tables.h
fillem.o:	types.h
gencode.o:	assert.h
gencode.o:	data.h
gencode.o:	extern.h
gencode.o:	param.h
gencode.o:	result.h
gencode.o:	tables.h
gencode.o:	types.h
glosym.o:	glosym.h
glosym.o:	param.h
glosym.o:	tables.h
glosym.o:	types.h
main.o:	param.h
move.o:	assert.h
move.o:	data.h
move.o:	extern.h
move.o:	param.h
move.o:	result.h
move.o:	tables.h
move.o:	types.h
nextem.o:	assert.h
nextem.o:	data.h
nextem.o:	extern.h
nextem.o:	param.h
nextem.o:	result.h
nextem.o:	tables.h
nextem.o:	types.h
reg.o:	assert.h
reg.o:	data.h
reg.o:	extern.h
reg.o:	param.h
reg.o:	result.h
reg.o:	tables.h
reg.o:	types.h
regvar.o:	assert.h
regvar.o:	data.h
regvar.o:	extern.h
regvar.o:	param.h
regvar.o:	regvar.h
regvar.o:	result.h
regvar.o:	tables.h
regvar.o:	types.h
salloc.o:	assert.h
salloc.o:	data.h
salloc.o:	extern.h
salloc.o:	param.h
salloc.o:	result.h
salloc.o:	tables.h
salloc.o:	types.h
state.o:	assert.h
state.o:	data.h
state.o:	extern.h
state.o:	param.h
state.o:	result.h
state.o:	state.h
state.o:	tables.h
state.o:	types.h
subr.o:	assert.h
subr.o:	data.h
subr.o:	extern.h
subr.o:	param.h
subr.o:	result.h
subr.o:	tables.h
subr.o:	types.h
var.o:	data.h
var.o:	param.h
var.o:	result.h
var.o:	tables.h
var.o:	types.h
