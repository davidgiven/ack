EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHR=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH)
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
classdefs.h pop_push.h alloc.o cset.o debug.o files.o go.o\
global.o lset.o map.o parser.o get.o put.o aux.o stack_chg.o locals.o\
init_glob.o

install:	all

cmp:		all

distr:		pop_push.h

em_files:\
classdefs.h pop_push.h alloc.m cset.m debug.m\
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
	rm -f makeclassdef classdefs.h *.o Out out nohup.out *.m

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
aux.o:	alloc.h
aux.o:	aux.h
aux.o:	debug.h
aux.o:	global.h
aux.o:	lset.h
aux.o:	map.h
aux.o:	types.h
cset.o:	alloc.h
cset.o:	cset.h
cset.o:	debug.h
cset.o:	global.h
cset.o:	types.h
debug.o:	debug.h
debug.o:	def.h
debug.o:	global.h
debug.o:	types.h
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
go.o:	alloc.h
go.o:	debug.h
go.o:	files.h
go.o:	get.h
go.o:	global.h
go.o:	go.h
go.o:	lset.h
go.o:	map.h
go.o:	put.h
go.o:	types.h
init_glob.o:	alloc.h
init_glob.o:	debug.h
init_glob.o:	global.h
init_glob.o:	map.h
init_glob.o:	types.h
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
parser.o:	alloc.h
parser.o:	aux.h
parser.o:	classdefs.h
parser.o:	debug.h
parser.o:	global.h
parser.o:	lset.h
parser.o:	types.h
put.o:	alloc.h
put.o:	debug.h
put.o:	def.h
put.o:	global.h
put.o:	lset.h
put.o:	map.h
put.o:	put.h
put.o:	types.h
show.o:	def.h
show.o:	global.h
show.o:	types.h
stack_chg.o:	debug.h
stack_chg.o:	global.h
stack_chg.o:	pop_push.h
stack_chg.o:	types.h
