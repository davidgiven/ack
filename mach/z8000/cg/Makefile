# $Header$

PREFLAGS=-I../../../h -DNDEBUG
PFLAGS=
CFLAGS=$(PREFLAGS) $(PFLAGS) -O
LDFLAGS=-i $(PFLAGS)
LINTOPTS=-hbxac
LIBS=../../../lib/em_data.a
CDIR=../../proto/cg
CFILES=$(CDIR)/codegen.c $(CDIR)/compute.c $(CDIR)/equiv.c $(CDIR)/fillem.c \
       $(CDIR)/gencode.c $(CDIR)/glosym.c $(CDIR)/main.c $(CDIR)/move.c \
       $(CDIR)/nextem.c $(CDIR)/reg.c $(CDIR)/regvar.c $(CDIR)/salloc.c \
       $(CDIR)/state.c $(CDIR)/subr.c $(CDIR)/var.c
OFILES=codegen.o compute.o equiv.o fillem.o gencode.o glosym.o main.o\
       move.o nextem.o reg.o regvar.o salloc.o state.o subr.o var.o

all:
	make tables.c
	make cg

cg: tables.o $(OFILES)
	cc $(LDFLAGS) $(OFILES) tables.o $(LIBS) -o cg

tables.o: tables.c
	cc -c $(PREFLAGS) tables.c

install: all
	../install cg

cmp:	 all
	-../compare cg


tables.c: table
	-mv tables.h tables.h.save
	../../../lib/cpp -P table | ../../../lib/cgg > debug.out
	-if cmp -s tables.h.save tables.h; then mv tables.h.save tables.h; else exit 0; fi
	-if cmp -s /dev/null tables.h; then mv tables.h.save tables.h; else exit 0; fi

lint: $(CFILES)
	lint $(LINTOPTS) $(PREFLAGS) $(CFILES)
clean:
	rm -f *.o tables.c

codegen.o:	$(CDIR)assert.h
codegen.o:	$(CDIR)data.h
codegen.o:	$(CDIR)equiv.h
codegen.o:	$(CDIR)extern.h
codegen.o:	$(CDIR)param.h
codegen.o:	$(CDIR)result.h
codegen.o:	$(CDIR)state.h
codegen.o:	$(CDIR)tables.h
codegen.o:	$(CDIR)types.h
compute.o:	$(CDIR)assert.h
compute.o:	$(CDIR)data.h
compute.o:	$(CDIR)extern.h
compute.o:	$(CDIR)glosym.h
compute.o:	$(CDIR)param.h
compute.o:	$(CDIR)result.h
compute.o:	$(CDIR)tables.h
compute.o:	$(CDIR)types.h
equiv.o:	$(CDIR)assert.h
equiv.o:	$(CDIR)data.h
equiv.o:	$(CDIR)equiv.h
equiv.o:	$(CDIR)extern.h
equiv.o:	$(CDIR)param.h
equiv.o:	$(CDIR)result.h
equiv.o:	$(CDIR)tables.h
equiv.o:	$(CDIR)types.h
fillem.o:	$(CDIR)assert.h
fillem.o:	$(CDIR)data.h
fillem.o:	$(CDIR)extern.h
fillem.o:	mach.c
fillem.o:	mach.h
fillem.o:	$(CDIR)param.h
fillem.o:	$(CDIR)regvar.h
fillem.o:	$(CDIR)result.h
fillem.o:	$(CDIR)tables.h
fillem.o:	$(CDIR)types.h
gencode.o:	$(CDIR)assert.h
gencode.o:	$(CDIR)data.h
gencode.o:	$(CDIR)extern.h
gencode.o:	$(CDIR)param.h
gencode.o:	$(CDIR)result.h
gencode.o:	$(CDIR)tables.h
gencode.o:	$(CDIR)types.h
glosym.o:	$(CDIR)glosym.h
glosym.o:	$(CDIR)param.h
glosym.o:	$(CDIR)tables.h
glosym.o:	$(CDIR)types.h
main.o:	$(CDIR)param.h
move.o:	$(CDIR)assert.h
move.o:	$(CDIR)data.h
move.o:	$(CDIR)extern.h
move.o:	$(CDIR)param.h
move.o:	$(CDIR)result.h
move.o:	$(CDIR)tables.h
move.o:	$(CDIR)types.h
nextem.o:	$(CDIR)assert.h
nextem.o:	$(CDIR)data.h
nextem.o:	$(CDIR)extern.h
nextem.o:	$(CDIR)param.h
nextem.o:	$(CDIR)result.h
nextem.o:	$(CDIR)tables.h
nextem.o:	$(CDIR)types.h
reg.o:	$(CDIR)assert.h
reg.o:	$(CDIR)data.h
reg.o:	$(CDIR)extern.h
reg.o:	$(CDIR)param.h
reg.o:	$(CDIR)result.h
reg.o:	$(CDIR)tables.h
reg.o:	$(CDIR)types.h
regvar.o:	$(CDIR)assert.h
regvar.o:	$(CDIR)data.h
regvar.o:	$(CDIR)extern.h
regvar.o:	$(CDIR)param.h
regvar.o:	$(CDIR)regvar.h
regvar.o:	$(CDIR)result.h
regvar.o:	$(CDIR)tables.h
regvar.o:	$(CDIR)types.h
salloc.o:	$(CDIR)assert.h
salloc.o:	$(CDIR)data.h
salloc.o:	$(CDIR)extern.h
salloc.o:	$(CDIR)param.h
salloc.o:	$(CDIR)result.h
salloc.o:	$(CDIR)tables.h
salloc.o:	$(CDIR)types.h
state.o:	$(CDIR)assert.h
state.o:	$(CDIR)data.h
state.o:	$(CDIR)extern.h
state.o:	$(CDIR)param.h
state.o:	$(CDIR)result.h
state.o:	$(CDIR)state.h
state.o:	$(CDIR)tables.h
state.o:	$(CDIR)types.h
subr.o:	$(CDIR)assert.h
subr.o:	$(CDIR)data.h
subr.o:	$(CDIR)extern.h
subr.o:	$(CDIR)param.h
subr.o:	$(CDIR)result.h
subr.o:	$(CDIR)tables.h
subr.o:	$(CDIR)types.h
var.o:	$(CDIR)data.h
var.o:	$(CDIR)param.h
var.o:	$(CDIR)result.h
var.o:	$(CDIR)tables.h
var.o:	$(CDIR)types.h
