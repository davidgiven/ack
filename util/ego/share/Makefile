EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHARE=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

CFILES=\
debug.c global.c files.c go.c map.c aux.c get.c put.c alloc.c lset.c\
cset.c parser.c stack_chg.c locals.c init_glob.c
SRC=\
types.h def.h debug.h global.h files.h go.h map.h aux.h get.h put.h alloc.h\
lset.h cset.h parser.h stack_chg.h locals.h init_glob.h $(CFILES)

PRFILES=$(SRC)

.SUFFIXES: .m
.c.m:
	$(CC) -c.m $(CFLAGS) $<

all:\
classdefs.h pop_push.h wordlen.h alloc.o cset.o debug.o files.o go.o\
global.o lset.o map.o parser.o get.o put.o aux.o stack_chg.o locals.o\
init_glob.o

install:	all

cmp:		all

distr:		pop_push.h

em_files:\
classdefs.h pop_push.h wordlen.h alloc.m cset.m debug.m\
files.m go.m global.m lset.m map.m parser.m get.m put.m aux.m stack_chg.m\
locals.m init_glob.m

classdefs.h: \
	makeclassdef \
	cldefs.src
	 makeclassdef $(EMH)/em_mnem.h cldefs.src > classdefs.h

makeclassdef: \
	makecldef.c
	 $(CC) -o makeclassdef makecldef.c

pop_push.h: \
	$(EMHOME)/etc/em_table pop_push.awk
	 awk -f pop_push.awk < $(EMHOME)/etc/em_table > pop_push.h

wordlen.h: makewordlen 
	makewordlen > wordlen.h
	rm makewordlen 

makewordlen: makewlen.c
	$(CC) -o makewordlen makewlen.c

show: \
	show.c
	 $(CC) -o show show.c $(EMLIB)/em_data.a

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	@echo "make clean does not remove anything, because it is also done"
	@echo "by the installation script, and this directory should not be"
	@echo "cleaned. Type 'make oclean' instead."

oclean:
	rm -f makeclassdef classdefs.h makewordlen wordlen.h *.o Out out nohup.out *.m

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) $(CFILES)

print:	$(PRFILES)
	@pr $?
	@touch print

depend:
	$(SHR)/makedepend

# the next lines are generated automatically
# AUTOAUTOAUTOAUTOAUTOAUTO
alloc.o:	alloc.h
alloc.o:	debug.h
alloc.o:	types.h
aux.o:	../../../h/em_mes.h
aux.o:	../../../h/em_pseu.h
aux.o:	../share/alloc.h
aux.o:	../share/aux.h
aux.o:	../share/debug.h
aux.o:	../share/global.h
aux.o:	../share/map.h
aux.o:	../share/types.h
cset.o:	alloc.h
cset.o:	cset.h
cset.o:	debug.h
cset.o:	global.h
cset.o:	types.h
debug.o:	../../../h/em_spec.h
debug.o:	debug.h
debug.o:	def.h
debug.o:	global.h
debug.o:	types.h
get.o:	../../../h/em_flag.h
get.o:	../../../h/em_mes.h
get.o:	../../../h/em_mnem.h
get.o:	../../../h/em_pseu.h
get.o:	../../../h/em_spec.h
get.o:	alloc.h
get.o:	aux.h
get.o:	cset.h
get.o:	debug.h
get.o:	def.h
get.o:	get.h
get.o:	global.h
get.o:	lset.h
get.o:	map.h
get.o:	types.h
global.o:	types.h
go.o:	../share/alloc.h
go.o:	../share/debug.h
go.o:	../share/files.h
go.o:	../share/get.h
go.o:	../share/global.h
go.o:	../share/lset.h
go.o:	../share/map.h
go.o:	../share/put.h
go.o:	../share/types.h
init_glob.o:	../share/alloc.h
init_glob.o:	../share/debug.h
init_glob.o:	../share/global.h
init_glob.o:	../share/map.h
init_glob.o:	../share/types.h
locals.o:	../../../h/em_mes.h
locals.o:	../../../h/em_mnem.h
locals.o:	../../../h/em_pseu.h
locals.o:	../../../h/em_spec.h
locals.o:	alloc.h
locals.o:	aux.h
locals.o:	cset.h
locals.o:	debug.h
locals.o:	def.h
locals.o:	get.h
locals.o:	global.h
locals.o:	locals.h
locals.o:	lset.h
locals.o:	types.h
lset.o:	alloc.h
lset.o:	debug.h
lset.o:	lset.h
lset.o:	types.h
map.o:	map.h
map.o:	types.h
parser.o:	../../../h/em_mnem.h
parser.o:	../../../h/em_spec.h
parser.o:	alloc.h
parser.o:	aux.h
parser.o:	classdefs.h
parser.o:	debug.h
parser.o:	global.h
parser.o:	lset.h
parser.o:	types.h
put.o:	../../../h/em_pseu.h
put.o:	../../../h/em_spec.h
put.o:	alloc.h
put.o:	debug.h
put.o:	def.h
put.o:	global.h
put.o:	lset.h
put.o:	map.h
put.o:	put.h
put.o:	types.h
show.o:	../../../h/em_flag.h
show.o:	../../../h/em_pseu.h
show.o:	../../../h/em_spec.h
show.o:	../share/def.h
show.o:	../share/global.h
show.o:	../share/types.h
stack_chg.o:	../share/debug.h
stack_chg.o:	../share/global.h
stack_chg.o:	../share/types.h
stack_chg.o:	../../../h/em_mnem.h
stack_chg.o:	../../../h/em_spec.h
stack_chg.o:	pop_push.h

alloc.m:	alloc.h
alloc.m:	debug.h
alloc.m:	types.h
aux.m:	../../../h/em_mes.h
aux.m:	../../../h/em_pseu.h
aux.m:	../share/alloc.h
aux.m:	../share/aux.h
aux.m:	../share/debug.h
aux.m:	../share/global.h
aux.m:	../share/map.h
aux.m:	../share/types.h
cset.m:	alloc.h
cset.m:	cset.h
cset.m:	debug.h
cset.m:	global.h
cset.m:	types.h
debug.m:	../../../h/em_spec.h
debug.m:	debug.h
debug.m:	def.h
debug.m:	global.h
debug.m:	types.h
get.m:	../../../h/em_flag.h
get.m:	../../../h/em_mes.h
get.m:	../../../h/em_mnem.h
get.m:	../../../h/em_pseu.h
get.m:	../../../h/em_spec.h
get.m:	alloc.h
get.m:	aux.h
get.m:	cset.h
get.m:	debug.h
get.m:	def.h
get.m:	get.h
get.m:	global.h
get.m:	lset.h
get.m:	map.h
get.m:	types.h
global.m:	types.h
go.m:	../share/alloc.h
go.m:	../share/debug.h
go.m:	../share/files.h
go.m:	../share/get.h
go.m:	../share/global.h
go.m:	../share/lset.h
go.m:	../share/map.h
go.m:	../share/put.h
go.m:	../share/types.h
init_glob.m:	../share/alloc.h
init_glob.m:	../share/debug.h
init_glob.m:	../share/global.h
init_glob.m:	../share/map.h
init_glob.m:	../share/types.h
locals.m:	../../../h/em_mes.h
locals.m:	../../../h/em_mnem.h
locals.m:	../../../h/em_pseu.h
locals.m:	../../../h/em_spec.h
locals.m:	alloc.h
locals.m:	aux.h
locals.m:	cset.h
locals.m:	debug.h
locals.m:	def.h
locals.m:	get.h
locals.m:	global.h
locals.m:	locals.h
locals.m:	lset.h
locals.m:	types.h
lset.m:	alloc.h
lset.m:	debug.h
lset.m:	lset.h
lset.m:	types.h
map.m:	map.h
map.m:	types.h
parser.m:	../../../h/em_mnem.h
parser.m:	../../../h/em_spec.h
parser.m:	alloc.h
parser.m:	aux.h
parser.m:	classdefs.h
parser.m:	debug.h
parser.m:	global.h
parser.m:	lset.h
parser.m:	types.h
put.m:	../../../h/em_pseu.h
put.m:	../../../h/em_spec.h
put.m:	alloc.h
put.m:	debug.h
put.m:	def.h
put.m:	global.h
put.m:	lset.h
put.m:	map.h
put.m:	put.h
put.m:	types.h
show.m:	../../../h/em_flag.h
show.m:	../../../h/em_pseu.h
show.m:	../../../h/em_spec.h
show.m:	../share/def.h
show.m:	../share/global.h
show.m:	../share/types.h
stack_chg.m:	../share/debug.h
stack_chg.m:	../share/global.h
stack_chg.m:	../share/types.h
stack_chg.m:	../../../h/em_mnem.h
stack_chg.m:	../../../h/em_spec.h
stack_chg.m:	pop_push.h
