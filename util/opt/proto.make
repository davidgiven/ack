# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/opt

CFILES=	$(SRC_DIR)/main.c $(SRC_DIR)/getline.c $(SRC_DIR)/lookup.c \
	$(SRC_DIR)/var.c $(SRC_DIR)/process.c $(SRC_DIR)/backward.c \
	$(SRC_DIR)/util.c $(SRC_DIR)/alloc.c $(SRC_DIR)/putline.c \
	$(SRC_DIR)/cleanup.c $(SRC_DIR)/peephole.c $(SRC_DIR)/flow.c \
	$(SRC_DIR)/reg.c $(SRC_DIR)/tes.c pop_push.c

OFILES=	main.$(SUF) getline.$(SUF) lookup.$(SUF) var.$(SUF) process.$(SUF) backward.$(SUF) util.$(SUF)\
	alloc.$(SUF) putline.$(SUF) cleanup.$(SUF) peephole.$(SUF) flow.$(SUF) tes.$(SUF) pop_push.$(SUF)
ONOGLOB=regnoglob.$(SUF)
OGLOB=regglob.$(SUF)

LIBS=	$(TARGET_HOME)/lib.bin/em_data.$(LIBSUF)
ULIBS=	$(UTIL_HOME)/lib.bin/em_data.$(ULIBSUF)
INCLUDES=-I$(TARGET_HOME)/h -I$(SRC_DIR) -I.
UINCLUDES=-I$(TARGET_HOME)/h -I$(SRC_DIR) -I.
CFLAGS= -DNDEBUG $(INCLUDES) $(COPTIONS)
UCFLAGS= -DNDEBUG $(INCLUDES) $(UCOPTIONS)
LDFLAGS=$(LDOPTIONS)
ULDFLAGS=$(ULDOPTIONS)
LINTFLAGS=$(INCLUDES) -DNDEBUG $(LINTOPTIONS)
CPP=$(UTIL_HOME)/lib.bin/cpp

all:	opt opt2

opt:	$(OFILES) $(ONOGLOB) pattern.$(SUF) $(LIBS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OFILES) $(ONOGLOB) pattern.$(SUF) $(LIBS) -o opt

opt2:	$(OFILES) $(OGLOB) pattern.$(SUF) $(LIBS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OFILES) $(OGLOB) pattern.$(SUF) $(LIBS) -o opt2

test:	opt testopt
	testopt

cmp:	all
	-cmp opt $(TARGET_HOMTARGET_HOME)/lib.bin/em_opt
	-cmp opt2 $(TARGET_HOME)/lib.bin/em_opt2
	-cmp $(SRC_DIR)/em_opt.6 $(TARGET_HOME)/man/em_opt.6

install:all
	rm -f $(TARGET_HOME)/lib.bin/em_opt
	cp opt $(TARGET_HOME)/lib.bin/em_opt
	rm -f $(TARGET_HOME)/lib.bin/em_opt2
	cp opt2 $(TARGET_HOME)/lib.bin/em_opt2
	rm -f $(TARGET_HOME)/man/em_opt.6
	cp $(SRC_DIR)/em_opt.6 $(TARGET_HOME)/man/em_opt.6

pattern.c:	$(SRC_DIR)/patterns mktab
	$(CPP) $(SRC_DIR)/patterns | mktab > pattern.c

mktab:	mktab.$(USUF) $(ULIBS)
	$(UCC) $(ULDFLAGS) mktab.$(USUF) $(ULIBS) -o mktab

mktab.$(USUF):	scan.c $(SRC_DIR)/optim.h $(SRC_DIR)/param.h $(SRC_DIR)/pattern.h $(SRC_DIR)/types.h mktab.c
	$(UCC) -c $(UCFLAGS) mktab.c

mktab.c:	$(SRC_DIR)/mktab.y
	yacc $(SRC_DIR)/mktab.y && mv y.tab.c mktab.c

pop_push.c pop_push.h: $(SRC_HOME)/etc/em_table $(SRC_DIR)/pop_push.awk
	awk -f $(SRC_DIR)/pop_push.awk < $(SRC_HOME)/etc/em_table > pop_push.c

regglob.c:	$(SRC_DIR)/reg.c
	echo '#define GLOBAL_OPT' > regglob.c
	cat $(SRC_DIR)/reg.c >> regglob.c

regnoglob.c:	$(SRC_DIR)/reg.c
	cp $(SRC_DIR)/reg.c regnoglob.c

depend: pattern.c pop_push.c regglob.c regnoglob.c
	sed '/^#DEPENDENCIES/,$$d' Makefile >Makefile.new
	echo '#DEPENDENCIES' >>Makefile.new
	for i in $(CFILES) pattern.c regglob.c regnoglob.c ; do \
		echo "`basename $$i .c`.$$(SUF):	$$i" >> Makefile.new ; \
		echo '	$$(CC) -c $$(CFLAGS)' $$i >> Makefile.new ; \
		$(UTIL_HOME)/lib.bin/cpp -d $(INCLUDES) $$i | sed "s/^/`basename $$i .c`.$$(SUF):	/" >> Makefile.new ; \
	done
	mv Makefile Makefile.old
	mv Makefile.new Makefile

lint:	$(CFILES) pattern.c
	$(LINT) $(LINTFLAGS) $(CFILES) pattern.c $(UTIL_HOME)/lib.bin/$(LINTPREF)em_data.$(LINTSUF)

opr:
	make pr | opr

pr:
	@pr -n $(SRC_DIR)/proto.make $(SRC_DIR)/*.h $(CFILES) $(SRC_DIR)/mktab.y $(SRC_DIR)/scan.l $(SRC_DIR)/patterns

clean:
	rm -f *.$(SUF) opt mktab mktab.c scan.c pattern.c opt2 Out \
		pop_push.c pop_push.h regglob.c regnoglob.c *.old

scan.c:	$(SRC_DIR)/scan.l
	flex -st $(SRC_DIR)/scan.l > scan.c

# the next lines are generated automatically
#DEPENDENCIES
main.$(SUF):	$(SRC_DIR)/main.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/main.c
main.$(SUF):	$(SRC_DIR)/ext.h
main.$(SUF):	$(TARGET_HOME)/h/em_spec.h
main.$(SUF):	$(SRC_DIR)/alloc.h
main.$(SUF):	$(SRC_DIR)/tes.h
main.$(SUF):	$(SRC_DIR)/types.h
main.$(SUF):	$(SRC_DIR)/param.h
getline.$(SUF):	$(SRC_DIR)/getline.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/getline.c
getline.$(SUF):	$(SRC_DIR)/ext.h
getline.$(SUF):	$(TARGET_HOME)/h/em_mes.h
getline.$(SUF):	$(TARGET_HOME)/h/em_flag.h
getline.$(SUF):	$(TARGET_HOME)/h/em_pseu.h
getline.$(SUF):	$(TARGET_HOME)/h/em_spec.h
getline.$(SUF):	$(SRC_DIR)/proinf.h
getline.$(SUF):	$(SRC_DIR)/alloc.h
getline.$(SUF):	$(SRC_DIR)/lookup.h
getline.$(SUF):	$(SRC_DIR)/line.h
getline.$(SUF):	$(SRC_DIR)/tes.h
getline.$(SUF):	$(SRC_DIR)/types.h
getline.$(SUF):	$(SRC_DIR)/param.h
lookup.$(SUF):	$(SRC_DIR)/lookup.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/lookup.c
lookup.$(SUF):	$(SRC_DIR)/proinf.h
lookup.$(SUF):	$(SRC_DIR)/alloc.h
lookup.$(SUF):	$(SRC_DIR)/lookup.h
lookup.$(SUF):	$(SRC_DIR)/tes.h
lookup.$(SUF):	$(SRC_DIR)/types.h
lookup.$(SUF):	$(SRC_DIR)/param.h
var.$(SUF):	$(SRC_DIR)/var.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/var.c
var.$(SUF):	$(SRC_DIR)/proinf.h
var.$(SUF):	$(SRC_DIR)/lookup.h
var.$(SUF):	$(SRC_DIR)/tes.h
var.$(SUF):	$(SRC_DIR)/types.h
var.$(SUF):	$(SRC_DIR)/param.h
process.$(SUF):	$(SRC_DIR)/process.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/process.c
process.$(SUF):	$(SRC_DIR)/ext.h
process.$(SUF):	$(SRC_DIR)/proinf.h
process.$(SUF):	$(SRC_DIR)/lookup.h
process.$(SUF):	$(SRC_DIR)/line.h
process.$(SUF):	$(SRC_DIR)/alloc.h
process.$(SUF):	$(TARGET_HOME)/h/em_pseu.h
process.$(SUF):	$(TARGET_HOME)/h/em_spec.h
process.$(SUF):	$(SRC_DIR)/assert.h
process.$(SUF):	$(SRC_DIR)/tes.h
process.$(SUF):	$(SRC_DIR)/types.h
process.$(SUF):	$(SRC_DIR)/param.h
backward.$(SUF):	$(SRC_DIR)/backward.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/backward.c
backward.$(SUF):	$(SRC_DIR)/ext.h
backward.$(SUF):	$(TARGET_HOME)/h/em_mes.h
backward.$(SUF):	$(TARGET_HOME)/h/em_mnem.h
backward.$(SUF):	$(TARGET_HOME)/h/em_pseu.h
backward.$(SUF):	$(TARGET_HOME)/h/em_spec.h
backward.$(SUF):	$(SRC_DIR)/proinf.h
backward.$(SUF):	$(SRC_DIR)/alloc.h
backward.$(SUF):	$(SRC_DIR)/lookup.h
backward.$(SUF):	$(SRC_DIR)/line.h
backward.$(SUF):	$(SRC_DIR)/assert.h
backward.$(SUF):	$(SRC_DIR)/tes.h
backward.$(SUF):	$(SRC_DIR)/types.h
backward.$(SUF):	$(SRC_DIR)/param.h
util.$(SUF):	$(SRC_DIR)/util.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/util.c
util.$(SUF):	$(SRC_DIR)/ext.h
util.$(SUF):	$(SRC_DIR)/optim.h
util.$(SUF):	$(SRC_DIR)/proinf.h
util.$(SUF):	$(SRC_DIR)/lookup.h
util.$(SUF):	$(SRC_DIR)/assert.h
util.$(SUF):	$(SRC_DIR)/tes.h
util.$(SUF):	$(SRC_DIR)/types.h
util.$(SUF):	$(SRC_DIR)/param.h
alloc.$(SUF):	$(SRC_DIR)/alloc.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/alloc.c
alloc.$(SUF):	$(SRC_DIR)/proinf.h
alloc.$(SUF):	$(SRC_DIR)/lookup.h
alloc.$(SUF):	$(SRC_DIR)/line.h
alloc.$(SUF):	$(SRC_DIR)/alloc.h
alloc.$(SUF):	$(SRC_DIR)/assert.h
alloc.$(SUF):	$(SRC_DIR)/tes.h
alloc.$(SUF):	$(SRC_DIR)/types.h
alloc.$(SUF):	$(SRC_DIR)/param.h
putline.$(SUF):	$(SRC_DIR)/putline.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/putline.c
putline.$(SUF):	$(SRC_DIR)/ext.h
putline.$(SUF):	$(SRC_DIR)/optim.h
putline.$(SUF):	$(SRC_DIR)/proinf.h
putline.$(SUF):	$(SRC_DIR)/lookup.h
putline.$(SUF):	$(SRC_DIR)/line.h
putline.$(SUF):	$(SRC_DIR)/alloc.h
putline.$(SUF):	$(TARGET_HOME)/h/em_flag.h
putline.$(SUF):	$(TARGET_HOME)/h/em_mnem.h
putline.$(SUF):	$(TARGET_HOME)/h/em_pseu.h
putline.$(SUF):	$(TARGET_HOME)/h/em_spec.h
putline.$(SUF):	$(SRC_DIR)/assert.h
putline.$(SUF):	$(SRC_DIR)/tes.h
putline.$(SUF):	$(SRC_DIR)/types.h
putline.$(SUF):	$(SRC_DIR)/param.h
cleanup.$(SUF):	$(SRC_DIR)/cleanup.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cleanup.c
cleanup.$(SUF):	$(SRC_DIR)/ext.h
cleanup.$(SUF):	$(SRC_DIR)/lookup.h
cleanup.$(SUF):	$(TARGET_HOME)/h/em_mes.h
cleanup.$(SUF):	$(TARGET_HOME)/h/em_spec.h
cleanup.$(SUF):	$(TARGET_HOME)/h/em_pseu.h
cleanup.$(SUF):	$(SRC_DIR)/assert.h
cleanup.$(SUF):	$(SRC_DIR)/types.h
cleanup.$(SUF):	$(SRC_DIR)/param.h
peephole.$(SUF):	$(SRC_DIR)/peephole.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/peephole.c
peephole.$(SUF):	$(SRC_DIR)/ext.h
peephole.$(SUF):	$(SRC_DIR)/optim.h
peephole.$(SUF):	$(TARGET_HOME)/h/em_mnem.h
peephole.$(SUF):	$(TARGET_HOME)/h/em_spec.h
peephole.$(SUF):	$(SRC_DIR)/pattern.h
peephole.$(SUF):	$(SRC_DIR)/alloc.h
peephole.$(SUF):	$(SRC_DIR)/proinf.h
peephole.$(SUF):	$(SRC_DIR)/lookup.h
peephole.$(SUF):	$(SRC_DIR)/line.h
peephole.$(SUF):	$(SRC_DIR)/assert.h
peephole.$(SUF):	$(SRC_DIR)/tes.h
peephole.$(SUF):	$(SRC_DIR)/types.h
peephole.$(SUF):	$(SRC_DIR)/param.h
flow.$(SUF):	$(SRC_DIR)/flow.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/flow.c
flow.$(SUF):	$(SRC_DIR)/ext.h
flow.$(SUF):	$(SRC_DIR)/optim.h
flow.$(SUF):	$(SRC_DIR)/proinf.h
flow.$(SUF):	$(SRC_DIR)/line.h
flow.$(SUF):	$(SRC_DIR)/alloc.h
flow.$(SUF):	$(TARGET_HOME)/h/em_mnem.h
flow.$(SUF):	$(TARGET_HOME)/h/em_spec.h
flow.$(SUF):	$(TARGET_HOME)/h/em_flag.h
flow.$(SUF):	$(SRC_DIR)/tes.h
flow.$(SUF):	$(SRC_DIR)/types.h
flow.$(SUF):	$(SRC_DIR)/param.h
reg.$(SUF):	$(SRC_DIR)/reg.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/reg.c
reg.$(SUF):	$(SRC_DIR)/ext.h
reg.$(SUF):	$(TARGET_HOME)/h/em_mes.h
reg.$(SUF):	$(TARGET_HOME)/h/em_pseu.h
reg.$(SUF):	$(TARGET_HOME)/h/em_spec.h
reg.$(SUF):	$(SRC_DIR)/alloc.h
reg.$(SUF):	$(SRC_DIR)/proinf.h
reg.$(SUF):	$(SRC_DIR)/tes.h
reg.$(SUF):	$(SRC_DIR)/line.h
reg.$(SUF):	$(SRC_DIR)/types.h
reg.$(SUF):	$(SRC_DIR)/param.h
reg.$(SUF):	$(SRC_DIR)/assert.h
tes.$(SUF):	$(SRC_DIR)/tes.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/tes.c
tes.$(SUF):	./pop_push.h
tes.$(SUF):	$(SRC_DIR)/ext.h
tes.$(SUF):	$(SRC_DIR)/line.h
tes.$(SUF):	$(SRC_DIR)/proinf.h
tes.$(SUF):	$(SRC_DIR)/alloc.h
tes.$(SUF):	$(SRC_DIR)/tes.h
tes.$(SUF):	$(SRC_DIR)/types.h
tes.$(SUF):	$(SRC_DIR)/assert.h
tes.$(SUF):	$(SRC_DIR)/param.h
tes.$(SUF):	$(TARGET_HOME)/h/em_pseu.h
tes.$(SUF):	$(TARGET_HOME)/h/em_mnem.h
tes.$(SUF):	$(TARGET_HOME)/h/em_spec.h
pop_push.$(SUF):	pop_push.c
	$(CC) -c $(CFLAGS) pop_push.c
pop_push.$(SUF):	pop_push.h
pattern.$(SUF):	pattern.c
	$(CC) -c $(CFLAGS) pattern.c
pattern.$(SUF):	$(SRC_DIR)/pattern.h
pattern.$(SUF):	$(SRC_DIR)/types.h
pattern.$(SUF):	$(SRC_DIR)/param.h
regglob.$(SUF):	regglob.c
	$(CC) -c $(CFLAGS) regglob.c
regglob.$(SUF):	$(SRC_DIR)/ext.h
regglob.$(SUF):	$(TARGET_HOME)/h/em_mes.h
regglob.$(SUF):	$(TARGET_HOME)/h/em_pseu.h
regglob.$(SUF):	$(TARGET_HOME)/h/em_spec.h
regglob.$(SUF):	$(SRC_DIR)/alloc.h
regglob.$(SUF):	$(SRC_DIR)/proinf.h
regglob.$(SUF):	$(SRC_DIR)/tes.h
regglob.$(SUF):	$(SRC_DIR)/line.h
regglob.$(SUF):	$(SRC_DIR)/types.h
regglob.$(SUF):	$(SRC_DIR)/param.h
regglob.$(SUF):	$(SRC_DIR)/assert.h
regnoglob.$(SUF):	regnoglob.c
	$(CC) -c $(CFLAGS) regnoglob.c
regnoglob.$(SUF):	$(SRC_DIR)/ext.h
regnoglob.$(SUF):	$(TARGET_HOME)/h/em_mes.h
regnoglob.$(SUF):	$(TARGET_HOME)/h/em_pseu.h
regnoglob.$(SUF):	$(TARGET_HOME)/h/em_spec.h
regnoglob.$(SUF):	$(SRC_DIR)/alloc.h
regnoglob.$(SUF):	$(SRC_DIR)/proinf.h
regnoglob.$(SUF):	$(SRC_DIR)/tes.h
regnoglob.$(SUF):	$(SRC_DIR)/line.h
regnoglob.$(SUF):	$(SRC_DIR)/types.h
regnoglob.$(SUF):	$(SRC_DIR)/param.h
regnoglob.$(SUF):	$(SRC_DIR)/assert.h
