# $Header$

EM =		../..#		# EM tree

CC =		cc#		# C comp used for compiling the interpreter
CFLAGS =	-O#		# passed to C compiler
LFLAGS =	#		# passed to loader

IDIRS =		-I$(EM)/h#	# passed to C compiler and lint

INT =		./int#		# name of resulting interpreter

IP_SPEC =	$(EM)/etc/ip_spec.t
TRAPS =		$(EM)/etc/traps
APP_A =		$(EM)/doc/int/appA	# to be moved later

SRC =	alloc.c core.c data.c do_array.c do_branch.c do_comp.c do_conv.c \
	do_fpar.c do_incdec.c do_intar.c do_load.c do_logic.c do_misc.c \
	do_proc.c do_ptrar.c do_sets.c do_store.c do_unsar.c dump.c \
	disassemble.c fra.c global.c init.c io.c log.c m_ioctl.c m_sigtrp.c \
	main.c moncalls.c monstruct.c proctab.c read.c rsb.c segment.c \
	stack.c switch.c tally.c text.c trap.c warn.c

OBJ =	alloc.o core.o data.o do_array.o do_branch.o do_comp.o do_conv.o \
	do_fpar.o do_incdec.o do_intar.o do_load.o do_logic.o do_misc.o \
	do_proc.o do_ptrar.o do_sets.o do_store.o do_unsar.o dump.o \
	disassemble.o fra.o global.o init.o io.o log.o m_ioctl.o m_sigtrp.o \
	main.o moncalls.o monstruct.o proctab.o read.o rsb.o segment.o \
	stack.o switch.o tally.o text.o trap.o warn.o

HDR =	alloc.h fra.h global.h linfil.h log.h mem.h memdirect.h monstruct.h \
	opcode.h proctab.h read.h rsb.h shadow.h text.h trap.h \
	logging.h debug.h nofloat.h segcheck.h sysidf.h v7ioctl.h \
	e.out.h#	should be in $(EM)/h or so, or in $(EM/h/em_abs.h

.SUFFIXES:	.o
.c.o:
	$(CC) $(CFLAGS) $(IDIRS) -c $<


# Main entries
all:	test

install:	$(INT)
	cp $(INT) $(EM)/bin/int
	cp int.1 $(EM)/man/int.1

cmp:	$(INT)
	-cmp $(INT) $(EM)/bin/int
	-cmp int.1 $(EM)/man/int.1

test:	$(INT) test/awa.em22 test/awa.em24 test/awa.em44
	@rm -f int.mess
	-echo 3 5 7 2 -1 | time $(INT) test/awa.em22
	cat int.mess
	@rm -f int.mess
	-echo 3 5 7 2 -1 | time $(INT) test/awa.em24
	cat int.mess
	@rm -f int.mess
	-echo 3 5 7 2 -1 | time $(INT) test/awa.em44
	cat int.mess

$(INT):	$(OBJ) Makefile
	$(CC) $(LFLAGS) -o $(INT) $(OBJ)
	@size $(INT)


# Generated files
trap_msg:	M.trap_msg $(TRAPS)
	M.trap_msg $(TRAPS)

warn_msg:	M.warn_msg $(APP_A)
	M.warn_msg $(APP_A)

warn.h:		M.warn_h $(APP_A)
	M.warn_h $(APP_A)

switch/DoCases:
	(cd switch; make DoCases)

switch/PrCases:	
	(cd switch; make PrCases)

test/awa.em22:
	(cd test; make awa.em22)

test/awa.em24:
	(cd test; make awa.em24)

test/awa.em44:
	(cd test; make awa.em44)


# Auxiliary entries
lint:
	lint $(IDIRS) $(SRC) -lc

tags:	$(HDR) $(SRC)
	ctags $(HDR) $(SRC)

MFILES =	M.trap_msg M.warn_h M.warn_msg

ALL =	READ_ME Makefile $(MFILES) $(HDR) $(SRC)
DISTR =	$(ALL) int.1 switch test

print:
	@pr $(ALL)

pr:	print

opr:
	make pr | opr

distr:	.distr

.distr:		Makefile
	echo $(DISTR) | tr ' ' '\012' >.distr

clean:
	rm -f core mon.out int.mess int.log int.core int.tally \
		trap_msg warn_msg warn.h tags print \
		$(OBJ)
	(cd switch; make clean)
	(cd switch; make clean)

bare:	clean
	/bin/rm -f $(INT)
	(cd switch; make bare)


#----------------------------------------------------------------
alloc.o: alloc.h debug.h global.h
core.o: fra.h global.h logging.h shadow.h
data.o: alloc.h global.h log.h logging.h mem.h memdirect.h nofloat.h shadow.h trap.h warn.h
disassemble.o: alloc.h global.h memdirect.h opcode.h proctab.h switch/PrCases
do_array.o: fra.h global.h log.h logging.h mem.h text.h trap.h
do_branch.o: fra.h global.h log.h logging.h mem.h text.h trap.h warn.h
do_comp.o: fra.h global.h log.h logging.h mem.h nofloat.h shadow.h text.h trap.h warn.h
do_conv.o: fra.h global.h log.h logging.h mem.h nofloat.h text.h trap.h warn.h
do_fpar.o: fra.h global.h log.h logging.h mem.h nofloat.h text.h trap.h warn.h
do_incdec.o: fra.h global.h log.h logging.h mem.h nofloat.h text.h trap.h warn.h
do_intar.o: fra.h global.h log.h logging.h mem.h text.h trap.h warn.h
do_load.o: fra.h global.h log.h logging.h mem.h rsb.h text.h trap.h warn.h
do_logic.o: fra.h global.h log.h logging.h mem.h shadow.h text.h trap.h warn.h
do_misc.o: fra.h global.h linfil.h log.h logging.h mem.h memdirect.h read.h rsb.h shadow.h text.h trap.h warn.h
do_proc.o: fra.h global.h linfil.h log.h logging.h mem.h memdirect.h proctab.h rsb.h shadow.h text.h trap.h warn.h
do_ptrar.o: fra.h global.h log.h logging.h mem.h segcheck.h text.h trap.h warn.h
do_sets.o: fra.h global.h log.h logging.h mem.h text.h trap.h
do_store.o: fra.h global.h log.h logging.h mem.h text.h trap.h warn.h
do_unsar.o: fra.h global.h log.h logging.h mem.h text.h trap.h warn.h
dump.o: fra.h global.h linfil.h log.h logging.h mem.h memdirect.h proctab.h rsb.h shadow.h text.h
fra.o: alloc.h fra.h global.h logging.h mem.h shadow.h
global.o: global.h
init.o: alloc.h global.h log.h logging.h mem.h read.h shadow.h trap.h warn.h
io.o: global.h linfil.h logging.h mem.h
log.o: global.h linfil.h logging.h
m_ioctl.o: global.h mem.h sysidf.h v7ioctl.h warn.h
m_sigtrp.o: global.h log.h logging.h trap.h warn.h
main.o: e.out.h global.h log.h logging.h nofloat.h opcode.h read.h rsb.h text.h trap.h warn.h
moncalls.o: alloc.h global.h log.h logging.h mem.h shadow.h sysidf.h trap.h warn.h
monstruct.o: global.h mem.h monstruct.h sysidf.h v7ioctl.h
proctab.o: alloc.h global.h log.h logging.h proctab.h
read.o: e.out.h global.h log.h logging.h mem.h nofloat.h read.h shadow.h text.h warn.h
rsb.o: global.h linfil.h logging.h mem.h proctab.h rsb.h shadow.h warn.h
segment.o: alloc.h global.h mem.h segcheck.h
stack.o: alloc.h global.h log.h logging.h mem.h memdirect.h nofloat.h rsb.h shadow.h trap.h warn.h
switch.o: global.h opcode.h switch/DoCases text.h trap.h warn.h
tally.o: alloc.h global.h linfil.h
text.o: alloc.h global.h proctab.h read.h text.h trap.h warn.h
trap.o: fra.h global.h linfil.h log.h logging.h mem.h rsb.h shadow.h trap.h trap_msg warn.h
warn.o: alloc.h global.h linfil.h log.h logging.h warn.h warn_msg
