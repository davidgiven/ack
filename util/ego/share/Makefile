EM=../../..
EMH=$(EM)/h
EML=$(EM)/lib
CFLAGS=-DVERBOSE -DNOTCOMPACT -O
SRC=types.h def.h debug.h debug.c global.h global.c files.h files.c go.h go.c map.h map.c aux.h aux.c get.h get.c put.h put.c alloc.h alloc.c lset.h lset.c cset.h cset.c parser.h parser.c stack_chg.h stack_chg.c locals.h locals.c init_glob.h init_glob.c

.SUFFIXES: .m
.c.m:
	ack -O -L -c.m $(CFLAGS) $<
all:	classdefs.h alloc.o cset.o debug.o files.o go.o global.o lset.o map.o parser.o get.o put.o aux.o stack_chg.o locals.o init_glob.o
optim:	classdefs.h alloc.m cset.m debug.m files.m global.m lset.m map.m parser.m get.m put.m stack_chg.m locals.m init_globl.m
classdefs.h: \
	makeclassdef \
	cldefs.src
	 makeclassdef $(EMH)/em_mnem.h cldefs.src > classdefs.h
makeclassdef: \
	makecldef.c
	 $(CC) -o makeclassdef makecldef.c
pop_push.h: \
	$(EM)/etc/em_table pop_push.awk
	 awk -f pop_push.awk < $(EM)/etc/em_table > pop_push.h
lpr:
	pr $(SRC) | lpr
opr:
	pr $(SRC) | opr
dumpflop:
	tar -uf ../../../ego/share/share.tarf $(SRC) Makefile show.c
show: \
	show.c
	 $(CC) -o show show.c $(EML)/em_data.a
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
