# $Header$

#PARAMS		do not remove this line

SRC_DIR = $(SRC_HOME)/util/int

INCLUDES = -I$(TARGET_HOME)/h -I$(TARGET_HOME)/config -I.
CFLAGS = $(COPTIONS) $(INCLUDES)
LDFLAGS = $(LDOPTIONS)
LINTFLAGS = $(LINTOPTIONS) $(INCLUDES)

INT =		int

IP_SPEC =	$(SRC_HOME)/etc/ip_spec.t
TRAPS =		$(SRC_HOME)/etc/traps
APP_A =		$(SRC_HOME)/doc/int/appA	# to be moved later

CFILES =	$(SRC_DIR)/alloc.c \
	$(SRC_DIR)/core.c \
	$(SRC_DIR)/data.c \
	$(SRC_DIR)/do_array.c \
	$(SRC_DIR)/do_branch.c \
	$(SRC_DIR)/do_comp.c \
	$(SRC_DIR)/do_conv.c \
	$(SRC_DIR)/do_fpar.c \
	$(SRC_DIR)/do_incdec.c \
	$(SRC_DIR)/do_intar.c \
	$(SRC_DIR)/do_load.c \
	$(SRC_DIR)/do_logic.c \
	$(SRC_DIR)/do_misc.c \
	$(SRC_DIR)/do_proc.c \
	$(SRC_DIR)/do_ptrar.c \
	$(SRC_DIR)/do_sets.c \
	$(SRC_DIR)/do_store.c \
	$(SRC_DIR)/do_unsar.c \
	$(SRC_DIR)/dump.c \
	$(SRC_DIR)/disassemble.c \
	$(SRC_DIR)/fra.c \
	$(SRC_DIR)/global.c \
	$(SRC_DIR)/init.c \
	$(SRC_DIR)/io.c \
	$(SRC_DIR)/log.c \
	$(SRC_DIR)/m_ioctl.c \
	$(SRC_DIR)/m_sigtrp.c \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/moncalls.c \
	$(SRC_DIR)/monstruct.c \
	$(SRC_DIR)/proctab.c \
	$(SRC_DIR)/read.c \
	$(SRC_DIR)/rsb.c \
	$(SRC_DIR)/segment.c \
	$(SRC_DIR)/stack.c \
	$(SRC_DIR)/switch.c \
	$(SRC_DIR)/tally.c \
	$(SRC_DIR)/text.c \
	$(SRC_DIR)/trap.c \
	$(SRC_DIR)/warn.c

OBJ =	alloc.o core.o data.o do_array.o do_branch.o do_comp.o do_conv.o \
	do_fpar.o do_incdec.o do_intar.o do_load.o do_logic.o do_misc.o \
	do_proc.o do_ptrar.o do_sets.o do_store.o do_unsar.o dump.o \
	disassemble.o fra.o global.o init.o io.o log.o m_ioctl.o m_sigtrp.o \
	main.o moncalls.o monstruct.o proctab.o read.o rsb.o segment.o \
	stack.o switch.o tally.o text.o trap.o warn.o

HDR =	$(SRC_DIR)/alloc.h \
	$(SRC_DIR)/fra.h \
	$(SRC_DIR)/global.h \
	$(SRC_DIR)/linfil.h \
	$(SRC_DIR)/log.h \
	$(SRC_DIR)/mem.h \
	$(SRC_DIR)/memdirect.h \
	$(SRC_DIR)/monstruct.h \
	$(SRC_DIR)/opcode.h \
	$(SRC_DIR)/proctab.h \
	$(SRC_DIR)/read.h \
	$(SRC_DIR)/rsb.h \
	$(SRC_DIR)/shadow.h \
	$(SRC_DIR)/text.h \
	$(SRC_DIR)/trap.h \
	$(SRC_DIR)/logging.h \
	$(SRC_DIR)/debug.h \
	$(SRC_DIR)/nofloat.h \
	$(SRC_DIR)/segcheck.h \
	$(SRC_DIR)/sysidf.h \
	$(SRC_DIR)/v7ioctl.h \
	$(SRC_DIR)/e.out.h#	should be in $(EM)/h or so, or in $(EM/h/em_abs.h

# Main entries
all:	test

install:	$(INT)
	cp $(INT) $(TARGET_HOME)/bin/int
	cp $(SRC_DIR)/int.1 $(TARGET_HOME)/man/int.1

cmp:	$(INT)
	-cmp $(INT) $(TARGET_HOME)/bin/int
	-cmp $(SRC_DIR)/int.1 $(TARGET_HOME)/man/int.1

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

$(INT):	$(OBJ)
	$(CC) $(LDFLAGS) -o $(INT) $(OBJ)

# Generated files
trap_msg:	$(SRC_DIR)/M.trap_msg $(TRAPS)
	$(SRC_DIR)/M.trap_msg $(TRAPS)

warn_msg:	$(SRC_DIR)/M.warn_msg $(APP_A)
	$(SRC_DIR)/M.warn_msg $(APP_A)

warn.h:		$(SRC_DIR)/M.warn_h $(APP_A)
	$(SRC_DIR)/M.warn_h $(APP_A)

switch/DoCases:
	(cd switch; make DoCases)

switch/PrCases:	
	(cd switch; make PrCases)

test/awa.em22:
	cp $(SRC_DIR)/test/*.[pc] test
	(cd test; make awa.em22)

test/awa.em24:
	cp $(SRC_DIR)/test/*.[pc] test
	(cd test; make awa.em24)

test/awa.em44:
	cp $(SRC_DIR)/test/*.[pc] test
	(cd test; make awa.em44)


# Auxiliary entries
lint:	$(CFILES) trap_msg warn_msg warn.h switch/DoCases switch/PrCases
	$(LINT) $(LINTFLAGS) $(CFILES)

tags:	$(HDR) $(CFILES)
	ctags $(HDR) $(CFILES)

MFILES =	$(SRC_DIR)/M.trap_msg $(SRC_DIR)/M.warn_h $(SRC_DIR)/M.warn_msg

ALL =	$(SRC_DIR)/READ_ME $(SRC_DIR)/proto.make $(MFILES) $(HDR) $(CFILES)

print:
	@pr $(ALL)

pr:	print

opr:
	make pr | opr

clean:
	rm -f core mon.out int.mess int.log int.core int.tally \
		trap_msg warn_msg warn.h tags print \
		$(OBJ) $(INT)
	(cd switch; make clean)
	(cd test; make clean)

bare:	clean
	(cd switch; make bare)

depend:	warn.h trap_msg warn_msg
	sed '/^#DEPENDENCIES/,$$d' Makefile >Makefile.new
	echo '#DEPENDENCIES' >>Makefile.new
	for i in $(CFILES) ; do \
		echo "`basename $$i .c`.$$(SUF):	$$i" >> Makefile.new ; \
		echo '	 $$(CC) -c $$(CFLAGS)' $$i >> Makefile.new ; \
		$(UTIL_HOME)/lib.bin/cpp -d $(INCLUDES) $$i | sed "s/^/`basename $$i .c`.$$(SUF):	/" >> Makefile.new ; \
	done
	mv Makefile Makefile.old
	mv Makefile.new Makefile

# do not remove the next line
#DEPENDENCIES
alloc.$(SUF):	$(SRC_DIR)/alloc.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/alloc.c
alloc.$(SUF):	$(SRC_DIR)/alloc.h
alloc.$(SUF):	$(SRC_DIR)/global.h
alloc.$(SUF):	$(SRC_DIR)/debug.h
core.$(SUF):	$(SRC_DIR)/core.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/core.c
core.$(SUF):	$(SRC_DIR)/fra.h
core.$(SUF):	$(SRC_DIR)/shadow.h
core.$(SUF):	$(SRC_DIR)/global.h
core.$(SUF):	$(SRC_DIR)/logging.h
data.$(SUF):	$(SRC_DIR)/data.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/data.c
data.$(SUF):	$(SRC_DIR)/shadow.h
data.$(SUF):	$(SRC_DIR)/mem.h
data.$(SUF):	$(SRC_DIR)/memdirect.h
data.$(SUF):	$(SRC_DIR)/alloc.h
data.$(SUF):	./warn.h
data.$(SUF):	$(SRC_DIR)/trap.h
data.$(SUF):	$(SRC_DIR)/log.h
data.$(SUF):	$(SRC_DIR)/global.h
data.$(SUF):	$(SRC_DIR)/nofloat.h
data.$(SUF):	$(SRC_DIR)/logging.h
data.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_array.$(SUF):	$(SRC_DIR)/do_array.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_array.c
do_array.$(SUF):	$(SRC_DIR)/fra.h
do_array.$(SUF):	$(SRC_DIR)/text.h
do_array.$(SUF):	$(SRC_DIR)/mem.h
do_array.$(SUF):	$(SRC_DIR)/trap.h
do_array.$(SUF):	$(SRC_DIR)/logging.h
do_array.$(SUF):	$(SRC_DIR)/log.h
do_array.$(SUF):	$(SRC_DIR)/global.h
do_array.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_branch.$(SUF):	$(SRC_DIR)/do_branch.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_branch.c
do_branch.$(SUF):	./warn.h
do_branch.$(SUF):	$(SRC_DIR)/fra.h
do_branch.$(SUF):	$(SRC_DIR)/text.h
do_branch.$(SUF):	$(SRC_DIR)/trap.h
do_branch.$(SUF):	$(SRC_DIR)/mem.h
do_branch.$(SUF):	$(SRC_DIR)/logging.h
do_branch.$(SUF):	$(SRC_DIR)/log.h
do_branch.$(SUF):	$(SRC_DIR)/global.h
do_branch.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_comp.$(SUF):	$(SRC_DIR)/do_comp.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_comp.c
do_comp.$(SUF):	$(SRC_DIR)/fra.h
do_comp.$(SUF):	$(SRC_DIR)/text.h
do_comp.$(SUF):	$(SRC_DIR)/trap.h
do_comp.$(SUF):	$(SRC_DIR)/shadow.h
do_comp.$(SUF):	$(SRC_DIR)/mem.h
do_comp.$(SUF):	./warn.h
do_comp.$(SUF):	$(SRC_DIR)/log.h
do_comp.$(SUF):	$(SRC_DIR)/global.h
do_comp.$(SUF):	$(SRC_DIR)/nofloat.h
do_comp.$(SUF):	$(SRC_DIR)/logging.h
do_comp.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_conv.$(SUF):	$(SRC_DIR)/do_conv.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_conv.c
do_conv.$(SUF):	./warn.h
do_conv.$(SUF):	$(SRC_DIR)/fra.h
do_conv.$(SUF):	$(SRC_DIR)/text.h
do_conv.$(SUF):	$(SRC_DIR)/trap.h
do_conv.$(SUF):	$(SRC_DIR)/mem.h
do_conv.$(SUF):	$(SRC_DIR)/logging.h
do_conv.$(SUF):	$(SRC_DIR)/log.h
do_conv.$(SUF):	$(SRC_DIR)/global.h
do_conv.$(SUF):	$(SRC_DIR)/nofloat.h
do_conv.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_fpar.$(SUF):	$(SRC_DIR)/do_fpar.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_fpar.c
do_fpar.$(SUF):	./warn.h
do_fpar.$(SUF):	$(SRC_DIR)/fra.h
do_fpar.$(SUF):	$(SRC_DIR)/text.h
do_fpar.$(SUF):	$(SRC_DIR)/trap.h
do_fpar.$(SUF):	$(SRC_DIR)/mem.h
do_fpar.$(SUF):	$(SRC_DIR)/logging.h
do_fpar.$(SUF):	$(SRC_DIR)/log.h
do_fpar.$(SUF):	$(SRC_DIR)/global.h
do_fpar.$(SUF):	$(SRC_DIR)/nofloat.h
do_fpar.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_incdec.$(SUF):	$(SRC_DIR)/do_incdec.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_incdec.c
do_incdec.$(SUF):	./warn.h
do_incdec.$(SUF):	$(SRC_DIR)/fra.h
do_incdec.$(SUF):	$(SRC_DIR)/text.h
do_incdec.$(SUF):	$(SRC_DIR)/mem.h
do_incdec.$(SUF):	$(SRC_DIR)/trap.h
do_incdec.$(SUF):	$(SRC_DIR)/nofloat.h
do_incdec.$(SUF):	$(SRC_DIR)/logging.h
do_incdec.$(SUF):	$(SRC_DIR)/log.h
do_incdec.$(SUF):	$(SRC_DIR)/global.h
do_incdec.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_intar.$(SUF):	$(SRC_DIR)/do_intar.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_intar.c
do_intar.$(SUF):	$(SRC_DIR)/fra.h
do_intar.$(SUF):	$(SRC_DIR)/text.h
do_intar.$(SUF):	./warn.h
do_intar.$(SUF):	$(SRC_DIR)/trap.h
do_intar.$(SUF):	$(SRC_DIR)/mem.h
do_intar.$(SUF):	$(SRC_DIR)/log.h
do_intar.$(SUF):	$(SRC_DIR)/global.h
do_intar.$(SUF):	$(SRC_DIR)/logging.h
do_intar.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_load.$(SUF):	$(SRC_DIR)/do_load.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_load.c
do_load.$(SUF):	./warn.h
do_load.$(SUF):	$(SRC_DIR)/rsb.h
do_load.$(SUF):	$(SRC_DIR)/fra.h
do_load.$(SUF):	$(SRC_DIR)/text.h
do_load.$(SUF):	$(SRC_DIR)/trap.h
do_load.$(SUF):	$(SRC_DIR)/mem.h
do_load.$(SUF):	$(SRC_DIR)/logging.h
do_load.$(SUF):	$(SRC_DIR)/log.h
do_load.$(SUF):	$(SRC_DIR)/global.h
do_load.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_logic.$(SUF):	$(SRC_DIR)/do_logic.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_logic.c
do_logic.$(SUF):	$(SRC_DIR)/fra.h
do_logic.$(SUF):	$(SRC_DIR)/text.h
do_logic.$(SUF):	$(SRC_DIR)/trap.h
do_logic.$(SUF):	$(SRC_DIR)/shadow.h
do_logic.$(SUF):	$(SRC_DIR)/mem.h
do_logic.$(SUF):	./warn.h
do_logic.$(SUF):	$(SRC_DIR)/log.h
do_logic.$(SUF):	$(SRC_DIR)/global.h
do_logic.$(SUF):	$(SRC_DIR)/logging.h
do_logic.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_misc.$(SUF):	$(SRC_DIR)/do_misc.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_misc.c
do_misc.$(SUF):	$(SRC_DIR)/linfil.h
do_misc.$(SUF):	$(SRC_DIR)/rsb.h
do_misc.$(SUF):	$(SRC_DIR)/fra.h
do_misc.$(SUF):	$(SRC_DIR)/read.h
do_misc.$(SUF):	$(SRC_DIR)/text.h
do_misc.$(SUF):	$(SRC_DIR)/shadow.h
do_misc.$(SUF):	$(SRC_DIR)/memdirect.h
do_misc.$(SUF):	$(SRC_DIR)/mem.h
do_misc.$(SUF):	./warn.h
do_misc.$(SUF):	$(SRC_DIR)/trap.h
do_misc.$(SUF):	$(SRC_DIR)/log.h
do_misc.$(SUF):	$(SRC_DIR)/global.h
do_misc.$(SUF):	$(SRC_DIR)/logging.h
do_misc.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_proc.$(SUF):	$(SRC_DIR)/do_proc.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_proc.c
do_proc.$(SUF):	$(SRC_DIR)/linfil.h
do_proc.$(SUF):	$(SRC_DIR)/rsb.h
do_proc.$(SUF):	$(SRC_DIR)/fra.h
do_proc.$(SUF):	$(SRC_DIR)/proctab.h
do_proc.$(SUF):	$(SRC_DIR)/text.h
do_proc.$(SUF):	./warn.h
do_proc.$(SUF):	$(SRC_DIR)/trap.h
do_proc.$(SUF):	$(SRC_DIR)/memdirect.h
do_proc.$(SUF):	$(SRC_DIR)/shadow.h
do_proc.$(SUF):	$(SRC_DIR)/mem.h
do_proc.$(SUF):	$(SRC_DIR)/log.h
do_proc.$(SUF):	$(SRC_DIR)/global.h
do_proc.$(SUF):	$(SRC_DIR)/logging.h
do_proc.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_ptrar.$(SUF):	$(SRC_DIR)/do_ptrar.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_ptrar.c
do_ptrar.$(SUF):	$(SRC_DIR)/fra.h
do_ptrar.$(SUF):	$(SRC_DIR)/text.h
do_ptrar.$(SUF):	./warn.h
do_ptrar.$(SUF):	$(SRC_DIR)/trap.h
do_ptrar.$(SUF):	$(SRC_DIR)/mem.h
do_ptrar.$(SUF):	$(SRC_DIR)/logging.h
do_ptrar.$(SUF):	$(SRC_DIR)/log.h
do_ptrar.$(SUF):	$(SRC_DIR)/global.h
do_ptrar.$(SUF):	$(SRC_DIR)/segcheck.h
do_ptrar.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_sets.$(SUF):	$(SRC_DIR)/do_sets.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_sets.c
do_sets.$(SUF):	$(SRC_DIR)/fra.h
do_sets.$(SUF):	$(SRC_DIR)/text.h
do_sets.$(SUF):	$(SRC_DIR)/mem.h
do_sets.$(SUF):	$(SRC_DIR)/trap.h
do_sets.$(SUF):	$(SRC_DIR)/logging.h
do_sets.$(SUF):	$(SRC_DIR)/log.h
do_sets.$(SUF):	$(SRC_DIR)/global.h
do_sets.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_store.$(SUF):	$(SRC_DIR)/do_store.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_store.c
do_store.$(SUF):	./warn.h
do_store.$(SUF):	$(SRC_DIR)/fra.h
do_store.$(SUF):	$(SRC_DIR)/text.h
do_store.$(SUF):	$(SRC_DIR)/trap.h
do_store.$(SUF):	$(SRC_DIR)/mem.h
do_store.$(SUF):	$(SRC_DIR)/logging.h
do_store.$(SUF):	$(SRC_DIR)/log.h
do_store.$(SUF):	$(SRC_DIR)/global.h
do_store.$(SUF):	$(TARGET_HOME)/h/em_abs.h
do_unsar.$(SUF):	$(SRC_DIR)/do_unsar.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/do_unsar.c
do_unsar.$(SUF):	$(SRC_DIR)/fra.h
do_unsar.$(SUF):	$(SRC_DIR)/text.h
do_unsar.$(SUF):	./warn.h
do_unsar.$(SUF):	$(SRC_DIR)/trap.h
do_unsar.$(SUF):	$(SRC_DIR)/mem.h
do_unsar.$(SUF):	$(SRC_DIR)/log.h
do_unsar.$(SUF):	$(SRC_DIR)/global.h
do_unsar.$(SUF):	$(SRC_DIR)/logging.h
do_unsar.$(SUF):	$(TARGET_HOME)/h/em_abs.h
dump.$(SUF):	$(SRC_DIR)/dump.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/dump.c
dump.$(SUF):	$(SRC_DIR)/rsb.h
dump.$(SUF):	$(SRC_DIR)/linfil.h
dump.$(SUF):	$(SRC_DIR)/shadow.h
dump.$(SUF):	$(SRC_DIR)/proctab.h
dump.$(SUF):	$(SRC_DIR)/text.h
dump.$(SUF):	$(SRC_DIR)/fra.h
dump.$(SUF):	$(SRC_DIR)/mem.h
dump.$(SUF):	$(SRC_DIR)/memdirect.h
dump.$(SUF):	$(SRC_DIR)/log.h
dump.$(SUF):	$(SRC_DIR)/global.h
dump.$(SUF):	$(SRC_DIR)/logging.h
dump.$(SUF):	$(TARGET_HOME)/h/em_abs.h
disassemble.$(SUF):	$(SRC_DIR)/disassemble.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/disassemble.c
disassemble.$(SUF):	./switch/PrCases
disassemble.$(SUF):	$(SRC_DIR)/alloc.h
disassemble.$(SUF):	$(SRC_DIR)/proctab.h
disassemble.$(SUF):	$(SRC_DIR)/memdirect.h
disassemble.$(SUF):	$(SRC_DIR)/opcode.h
disassemble.$(SUF):	$(SRC_DIR)/global.h
fra.$(SUF):	$(SRC_DIR)/fra.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/fra.c
fra.$(SUF):	$(SRC_DIR)/alloc.h
fra.$(SUF):	$(SRC_DIR)/fra.h
fra.$(SUF):	$(SRC_DIR)/shadow.h
fra.$(SUF):	$(SRC_DIR)/mem.h
fra.$(SUF):	$(SRC_DIR)/global.h
fra.$(SUF):	$(SRC_DIR)/logging.h
global.$(SUF):	$(SRC_DIR)/global.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/global.c
global.$(SUF):	$(SRC_DIR)/global.h
init.$(SUF):	$(SRC_DIR)/init.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/init.c
init.$(SUF):	$(SRC_DIR)/read.h
init.$(SUF):	$(SRC_DIR)/trap.h
init.$(SUF):	$(SRC_DIR)/shadow.h
init.$(SUF):	$(SRC_DIR)/mem.h
init.$(SUF):	./warn.h
init.$(SUF):	$(SRC_DIR)/alloc.h
init.$(SUF):	$(SRC_DIR)/log.h
init.$(SUF):	$(SRC_DIR)/global.h
init.$(SUF):	$(SRC_DIR)/logging.h
init.$(SUF):	$(TARGET_HOME)/h/em_abs.h
io.$(SUF):	$(SRC_DIR)/io.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/io.c
io.$(SUF):	$(SRC_DIR)/linfil.h
io.$(SUF):	$(SRC_DIR)/mem.h
io.$(SUF):	$(SRC_DIR)/global.h
io.$(SUF):	$(SRC_DIR)/logging.h
log.$(SUF):	$(SRC_DIR)/log.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/log.c
log.$(SUF):	$(SRC_DIR)/linfil.h
log.$(SUF):	$(SRC_DIR)/global.h
log.$(SUF):	$(SRC_DIR)/logging.h
m_ioctl.$(SUF):	$(SRC_DIR)/m_ioctl.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/m_ioctl.c
m_ioctl.$(SUF):	./warn.h
m_ioctl.$(SUF):	$(SRC_DIR)/mem.h
m_ioctl.$(SUF):	$(SRC_DIR)/global.h
m_ioctl.$(SUF):	$(SRC_DIR)/v7ioctl.h
m_ioctl.$(SUF):	$(TARGET_HOME)/config/local.h
m_ioctl.$(SUF):	$(SRC_DIR)/sysidf.h
m_sigtrp.$(SUF):	$(SRC_DIR)/m_sigtrp.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/m_sigtrp.c
m_sigtrp.$(SUF):	$(SRC_DIR)/trap.h
m_sigtrp.$(SUF):	./warn.h
m_sigtrp.$(SUF):	$(SRC_DIR)/logging.h
m_sigtrp.$(SUF):	$(SRC_DIR)/log.h
m_sigtrp.$(SUF):	$(SRC_DIR)/global.h
main.$(SUF):	$(SRC_DIR)/main.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/main.c
main.$(SUF):	$(SRC_DIR)/rsb.h
main.$(SUF):	$(SRC_DIR)/opcode.h
main.$(SUF):	$(SRC_DIR)/read.h
main.$(SUF):	$(SRC_DIR)/text.h
main.$(SUF):	./warn.h
main.$(SUF):	$(SRC_DIR)/trap.h
main.$(SUF):	$(SRC_DIR)/log.h
main.$(SUF):	$(SRC_DIR)/global.h
main.$(SUF):	$(SRC_DIR)/nofloat.h
main.$(SUF):	$(SRC_DIR)/logging.h
main.$(SUF):	$(SRC_DIR)/e.out.h
main.$(SUF):	$(TARGET_HOME)/h/em_abs.h
moncalls.$(SUF):	$(SRC_DIR)/moncalls.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/moncalls.c
moncalls.$(SUF):	$(SRC_DIR)/mem.h
moncalls.$(SUF):	./warn.h
moncalls.$(SUF):	$(SRC_DIR)/trap.h
moncalls.$(SUF):	$(SRC_DIR)/global.h
moncalls.$(SUF):	$(TARGET_HOME)/h/em_abs.h
moncalls.$(SUF):	$(SRC_DIR)/shadow.h
moncalls.$(SUF):	$(SRC_DIR)/alloc.h
moncalls.$(SUF):	$(SRC_DIR)/logging.h
moncalls.$(SUF):	$(SRC_DIR)/log.h
moncalls.$(SUF):	$(TARGET_HOME)/config/local.h
moncalls.$(SUF):	$(SRC_DIR)/sysidf.h
monstruct.$(SUF):	$(SRC_DIR)/monstruct.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/monstruct.c
monstruct.$(SUF):	$(SRC_DIR)/monstruct.h
monstruct.$(SUF):	$(SRC_DIR)/mem.h
monstruct.$(SUF):	$(SRC_DIR)/global.h
monstruct.$(SUF):	$(SRC_DIR)/v7ioctl.h
monstruct.$(SUF):	$(TARGET_HOME)/config/local.h
monstruct.$(SUF):	$(SRC_DIR)/sysidf.h
proctab.$(SUF):	$(SRC_DIR)/proctab.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/proctab.c
proctab.$(SUF):	$(SRC_DIR)/proctab.h
proctab.$(SUF):	$(SRC_DIR)/alloc.h
proctab.$(SUF):	$(SRC_DIR)/log.h
proctab.$(SUF):	$(SRC_DIR)/global.h
proctab.$(SUF):	$(SRC_DIR)/logging.h
read.$(SUF):	$(SRC_DIR)/read.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/read.c
read.$(SUF):	$(SRC_DIR)/text.h
read.$(SUF):	$(SRC_DIR)/read.h
read.$(SUF):	$(SRC_DIR)/shadow.h
read.$(SUF):	$(SRC_DIR)/mem.h
read.$(SUF):	./warn.h
read.$(SUF):	$(SRC_DIR)/log.h
read.$(SUF):	$(SRC_DIR)/global.h
read.$(SUF):	$(SRC_DIR)/nofloat.h
read.$(SUF):	$(SRC_DIR)/logging.h
read.$(SUF):	$(TARGET_HOME)/h/as_spec.h
read.$(SUF):	$(TARGET_HOME)/h/em_spec.h
read.$(SUF):	$(TARGET_HOME)/config/local.h
rsb.$(SUF):	$(SRC_DIR)/rsb.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/rsb.c
rsb.$(SUF):	./warn.h
rsb.$(SUF):	$(SRC_DIR)/shadow.h
rsb.$(SUF):	$(SRC_DIR)/linfil.h
rsb.$(SUF):	$(SRC_DIR)/proctab.h
rsb.$(SUF):	$(SRC_DIR)/rsb.h
rsb.$(SUF):	$(SRC_DIR)/mem.h
rsb.$(SUF):	$(SRC_DIR)/global.h
rsb.$(SUF):	$(SRC_DIR)/logging.h
segment.$(SUF):	$(SRC_DIR)/segment.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/segment.c
segment.$(SUF):	$(SRC_DIR)/alloc.h
segment.$(SUF):	$(SRC_DIR)/mem.h
segment.$(SUF):	$(SRC_DIR)/global.h
segment.$(SUF):	$(SRC_DIR)/segcheck.h
stack.$(SUF):	$(SRC_DIR)/stack.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/stack.c
stack.$(SUF):	$(SRC_DIR)/rsb.h
stack.$(SUF):	$(SRC_DIR)/shadow.h
stack.$(SUF):	$(SRC_DIR)/mem.h
stack.$(SUF):	$(SRC_DIR)/memdirect.h
stack.$(SUF):	$(SRC_DIR)/alloc.h
stack.$(SUF):	$(SRC_DIR)/trap.h
stack.$(SUF):	./warn.h
stack.$(SUF):	$(SRC_DIR)/log.h
stack.$(SUF):	$(SRC_DIR)/global.h
stack.$(SUF):	$(SRC_DIR)/nofloat.h
stack.$(SUF):	$(SRC_DIR)/logging.h
stack.$(SUF):	$(TARGET_HOME)/h/em_abs.h
switch.$(SUF):	$(SRC_DIR)/switch.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/switch.c
switch.$(SUF):	./switch/DoCases
switch.$(SUF):	./warn.h
switch.$(SUF):	$(SRC_DIR)/trap.h
switch.$(SUF):	$(SRC_DIR)/text.h
switch.$(SUF):	$(SRC_DIR)/opcode.h
switch.$(SUF):	$(SRC_DIR)/global.h
switch.$(SUF):	$(TARGET_HOME)/h/em_abs.h
tally.$(SUF):	$(SRC_DIR)/tally.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/tally.c
tally.$(SUF):	$(SRC_DIR)/alloc.h
tally.$(SUF):	$(SRC_DIR)/linfil.h
tally.$(SUF):	$(SRC_DIR)/global.h
text.$(SUF):	$(SRC_DIR)/text.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/text.c
text.$(SUF):	./warn.h
text.$(SUF):	$(SRC_DIR)/proctab.h
text.$(SUF):	$(SRC_DIR)/read.h
text.$(SUF):	$(SRC_DIR)/text.h
text.$(SUF):	$(SRC_DIR)/trap.h
text.$(SUF):	$(SRC_DIR)/alloc.h
text.$(SUF):	$(SRC_DIR)/global.h
text.$(SUF):	$(TARGET_HOME)/h/em_abs.h
trap.$(SUF):	$(SRC_DIR)/trap.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/trap.c
trap.$(SUF):	./trap_msg
trap.$(SUF):	$(SRC_DIR)/fra.h
trap.$(SUF):	$(SRC_DIR)/rsb.h
trap.$(SUF):	$(SRC_DIR)/linfil.h
trap.$(SUF):	$(SRC_DIR)/shadow.h
trap.$(SUF):	$(SRC_DIR)/mem.h
trap.$(SUF):	./warn.h
trap.$(SUF):	$(SRC_DIR)/trap.h
trap.$(SUF):	$(SRC_DIR)/log.h
trap.$(SUF):	$(SRC_DIR)/global.h
trap.$(SUF):	$(SRC_DIR)/logging.h
trap.$(SUF):	$(TARGET_HOME)/h/em_abs.h
warn.$(SUF):	$(SRC_DIR)/warn.c
	 $(CC) -c $(CFLAGS) $(SRC_DIR)/warn.c
warn.$(SUF):	./warn_msg
warn.$(SUF):	$(SRC_DIR)/linfil.h
warn.$(SUF):	./warn.h
warn.$(SUF):	$(SRC_DIR)/alloc.h
warn.$(SUF):	$(SRC_DIR)/log.h
warn.$(SUF):	$(SRC_DIR)/global.h
warn.$(SUF):	$(SRC_DIR)/logging.h
