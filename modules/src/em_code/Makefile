EMHOME = ../../..
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare

all:		libeme.a libemk.a em_code.3
		rm -f *.c

install:	all
		$(INSTALL) lib/libeme.a
		$(INSTALL) lib/libemk.a
		$(INSTALL) man/em_code.3

compare:	all
		$(COMPARE) lib/libeme.a
		$(COMPARE) lib/libemk.a
		$(COMPARE) man/em_code.3

em_code.3:	em_code.3X
		-sh -c 'tbl < em_code.3X > em_code.3'
		-sh -c 'if test -s em_code.3 ; then : ; else cp em_code.3X em_code.3 ; fi '

libeme.a:	make.sh e/em_private.h e/em.c e/C_failed.c
		EMHOME=$(EMHOME); export EMHOME; sh make.sh e
		-sh -c 'ranlib libeme.a'

libemk.a:	make.sh k/em_private.h k/em.c k/C_failed.c
		EMHOME=$(EMHOME); export EMHOME; sh make.sh k
		-sh -c 'ranlib libemk.a'

make.sh:	em.gen em.nogen
		make.fun em.gen em.nogen | sh

em.gen:		make.em.gen $(EMHOME)/etc/em_table
		make.em.gen $(EMHOME)/etc/em_table > em.gen

pr:
		@pr Makefile make.em.gen make.fun em.nogen e/em_private.h e/em.c k/em_private.h k/em.c

opr:
		make pr | opr

# don't put the next "rm"s all on one line. the argument list then
# becomes too long for some systems
clean:
		rm -f *.o
		rm -f *.c
		rm -f *.a em_code.3 em.gen make.sh
