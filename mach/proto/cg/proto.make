# $Id$

#PARAMS         do not remove this line!

#MACH_DEFINE	do not remove this or the next line!
MACH = 

SRC_DIR = $(SRC_HOME)/mach/$(MACH)/cg
CDIR=$(SRC_HOME)/mach/proto/cg
LIBEM = $(TARGET_HOME)/lib.bin/em_data.$(LIBSUF)
LIBFLT = $(TARGET_HOME)/modules/lib/libflt.$(LIBSUF)
CPP = $(UTIL_HOME)/lib.bin/cpp
CGG = $(UTIL_HOME)/lib.bin/cgg

PREFLAGS=-I$(TARGET_HOME)/h -I$(SRC_HOME)/mach -I$(TARGET_HOME)/modules/h -I$(SRC_DIR) -I. -I$(CDIR) -DNDEBUG
PFLAGS=
CFLAGS=$(PREFLAGS) $(PFLAGS) $(COPTIONS)
LDFLAGS=$(PFLAGS) $(LDOPTIONS)
LINTFLAGS=$(PREFLAGS) $(LINTOPTIONS)
LIBS=$(LIBEM) $(LIBFLT)
CFILES=$(CDIR)/codegen.c $(CDIR)/compute.c $(CDIR)/equiv.c $(CDIR)/fillem.c \
       $(CDIR)/gencode.c $(CDIR)/glosym.c $(CDIR)/main.c $(CDIR)/move.c \
       $(CDIR)/nextem.c $(CDIR)/reg.c $(CDIR)/regvar.c $(CDIR)/salloc.c \
       $(CDIR)/state.c $(CDIR)/subr.c $(CDIR)/var.c
OFILES=	codegen.$(SUF) compute.$(SUF) equiv.$(SUF) fillem.$(SUF) \
	gencode.$(SUF) glosym.$(SUF) main.$(SUF) move.$(SUF) nextem.$(SUF) \
	reg.$(SUF) regvar.$(SUF) salloc.$(SUF) state.$(SUF) subr.$(SUF) \
	var.$(SUF)

all:	tables.c
	make cg

cg: tables.o $(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) tables.o $(LIBS) -o cg

tables.$(SUF): tables.c
	$(CC) -c $(PREFLAGS) tables.c

codegen.$(SUF): $(CDIR)/codegen.c
	$(CC) -c $(CFLAGS) $(CDIR)/codegen.c
compute.$(SUF): $(CDIR)/compute.c
	$(CC) -c $(CFLAGS) $(CDIR)/compute.c
equiv.$(SUF): $(CDIR)/equiv.c
	$(CC) -c $(CFLAGS) $(CDIR)/equiv.c
fillem.$(SUF): $(CDIR)/fillem.c
	$(CC) -c $(CFLAGS) $(CDIR)/fillem.c
gencode.$(SUF): $(CDIR)/gencode.c
	$(CC) -c $(CFLAGS) $(CDIR)/gencode.c
glosym.$(SUF): $(CDIR)/glosym.c
	$(CC) -c $(CFLAGS) $(CDIR)/glosym.c
main.$(SUF): $(CDIR)/main.c
	$(CC) -c $(CFLAGS) $(CDIR)/main.c
move.$(SUF): $(CDIR)/move.c
	$(CC) -c $(CFLAGS) $(CDIR)/move.c
nextem.$(SUF): $(CDIR)/nextem.c
	$(CC) -c $(CFLAGS) $(CDIR)/nextem.c
reg.$(SUF): $(CDIR)/reg.c
	$(CC) -c $(CFLAGS) $(CDIR)/reg.c
regvar.$(SUF): $(CDIR)/regvar.c
	$(CC) -c $(CFLAGS) $(CDIR)/regvar.c
salloc.$(SUF): $(CDIR)/salloc.c
	$(CC) -c $(CFLAGS) $(CDIR)/salloc.c
state.$(SUF): $(CDIR)/state.c
	$(CC) -c $(CFLAGS) $(CDIR)/state.c
subr.$(SUF): $(CDIR)/subr.c
	$(CC) -c $(CFLAGS) $(CDIR)/subr.c
var.$(SUF): $(CDIR)/var.c
	$(CC) -c $(CFLAGS) $(CDIR)/var.c

install: all
	-mkdir $(TARGET_HOME)/lib.bin/$(MACH)
	cp cg $(TARGET_HOME)/lib.bin/$(MACH)/cg

cmp:	 all
	-cmp cg $(TARGET_HOME)/lib.bin/$(MACH)/cg

tables.c: $(SRC_DIR)/table $(CGG)
	-mv tables.h tables.h.save
	$(CPP) -P -I$(SRC_DIR) $(SRC_DIR)/table | $(CGG) > debug.out
	-if cmp -s tables.h.save tables.h; then mv tables.h.save tables.h; else exit 0; fi
	-if cmp -s /dev/null tables.h; then mv tables.h.save tables.h; else exit 0; fi

lint:	tables.c
	$(LINT) $(LINTFLAGS) $(CFILES) tables.c
clean:
	rm -f *.$(SUF) tables.c tables.h debug.out cg tables.h.save

codegen.$(SUF):	$(CDIR)/assert.h
codegen.$(SUF):	$(CDIR)/data.h
codegen.$(SUF):	$(CDIR)/equiv.h
codegen.$(SUF):	$(CDIR)/extern.h
codegen.$(SUF):	$(CDIR)/param.h
codegen.$(SUF):	$(CDIR)/result.h
codegen.$(SUF):	$(CDIR)/state.h
codegen.$(SUF):	tables.h
codegen.$(SUF):	$(CDIR)/types.h
compute.$(SUF):	$(CDIR)/assert.h
compute.$(SUF):	$(CDIR)/data.h
compute.$(SUF):	$(CDIR)/extern.h
compute.$(SUF):	$(CDIR)/glosym.h
compute.$(SUF):	$(CDIR)/param.h
compute.$(SUF):	$(CDIR)/result.h
compute.$(SUF):	tables.h
compute.$(SUF):	$(CDIR)/types.h
equiv.$(SUF):	$(CDIR)/assert.h
equiv.$(SUF):	$(CDIR)/data.h
equiv.$(SUF):	$(CDIR)/equiv.h
equiv.$(SUF):	$(CDIR)/extern.h
equiv.$(SUF):	$(CDIR)/param.h
equiv.$(SUF):	$(CDIR)/result.h
equiv.$(SUF):	tables.h
equiv.$(SUF):	$(CDIR)/types.h
fillem.$(SUF):	$(CDIR)/assert.h
fillem.$(SUF):	$(CDIR)/data.h
fillem.$(SUF):	$(CDIR)/extern.h
fillem.$(SUF):	$(SRC_DIR)/mach.c
fillem.$(SUF):	$(SRC_DIR)/mach.h
fillem.$(SUF):	$(CDIR)/param.h
fillem.$(SUF):	$(CDIR)/regvar.h
fillem.$(SUF):	$(CDIR)/result.h
fillem.$(SUF):	tables.h
fillem.$(SUF):	$(CDIR)/types.h
gencode.$(SUF):	$(CDIR)/assert.h
gencode.$(SUF):	$(CDIR)/data.h
gencode.$(SUF):	$(CDIR)/extern.h
gencode.$(SUF):	$(CDIR)/param.h
gencode.$(SUF):	$(CDIR)/result.h
gencode.$(SUF):	tables.h
gencode.$(SUF):	$(CDIR)/types.h
glosym.$(SUF):	$(CDIR)/glosym.h
glosym.$(SUF):	$(CDIR)/param.h
glosym.$(SUF):	tables.h
glosym.$(SUF):	$(CDIR)/types.h
main.$(SUF):		$(CDIR)/param.h
move.$(SUF):		$(CDIR)/assert.h
move.$(SUF):		$(CDIR)/data.h
move.$(SUF):		$(CDIR)/extern.h
move.$(SUF):		$(CDIR)/param.h
move.$(SUF):		$(CDIR)/result.h
move.$(SUF):		tables.h
move.$(SUF):		$(CDIR)/types.h
nextem.$(SUF):	$(CDIR)/assert.h
nextem.$(SUF):	$(CDIR)/data.h
nextem.$(SUF):	$(CDIR)/extern.h
nextem.$(SUF):	$(CDIR)/param.h
nextem.$(SUF):	$(CDIR)/result.h
nextem.$(SUF):	tables.h
nextem.$(SUF):	$(CDIR)/types.h
reg.$(SUF):		$(CDIR)/assert.h
reg.$(SUF):		$(CDIR)/data.h
reg.$(SUF):		$(CDIR)/extern.h
reg.$(SUF):		$(CDIR)/param.h
reg.$(SUF):		$(CDIR)/result.h
reg.$(SUF):		tables.h
reg.$(SUF):		$(CDIR)/types.h
regvar.$(SUF):	$(CDIR)/assert.h
regvar.$(SUF):	$(CDIR)/data.h
regvar.$(SUF):	$(CDIR)/extern.h
regvar.$(SUF):	$(CDIR)/param.h
regvar.$(SUF):	$(CDIR)/regvar.h
regvar.$(SUF):	$(CDIR)/result.h
regvar.$(SUF):	tables.h
regvar.$(SUF):	$(CDIR)/types.h
salloc.$(SUF):	$(CDIR)/assert.h
salloc.$(SUF):	$(CDIR)/data.h
salloc.$(SUF):	$(CDIR)/extern.h
salloc.$(SUF):	$(CDIR)/param.h
salloc.$(SUF):	$(CDIR)/result.h
salloc.$(SUF):	tables.h
salloc.$(SUF):	$(CDIR)/types.h
state.$(SUF):	$(CDIR)/assert.h
state.$(SUF):	$(CDIR)/data.h
state.$(SUF):	$(CDIR)/extern.h
state.$(SUF):	$(CDIR)/param.h
state.$(SUF):	$(CDIR)/result.h
state.$(SUF):	$(CDIR)/state.h
state.$(SUF):	tables.h
state.$(SUF):	$(CDIR)/types.h
subr.$(SUF):		$(CDIR)/assert.h
subr.$(SUF):		$(CDIR)/data.h
subr.$(SUF):		$(CDIR)/extern.h
subr.$(SUF):		$(CDIR)/param.h
subr.$(SUF):		$(CDIR)/result.h
subr.$(SUF):		tables.h
subr.$(SUF):		$(CDIR)/types.h
var.$(SUF):		$(CDIR)/data.h
var.$(SUF):		$(CDIR)/param.h
var.$(SUF):		$(CDIR)/result.h
var.$(SUF):		tables.h
var.$(SUF):		$(CDIR)/types.h
