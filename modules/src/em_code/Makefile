EMHOME = ../../..
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare
INCORE = -DINCORE
CFLAGS = -I$(EMHOME)/h -I$(EMHOME)/modules/h -O
OBS = failed.o insert.o internerr.o io.o getid.o

all:		libeme.a libemk.a em_code.3
		rm -f C_*.c

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

libeme.a:	make.sh e/em_private.h e/em.c $(OBS)
		EMHOME=$(EMHOME); export EMHOME; sh make.sh e
		-sh -c 'ranlib libeme.a'

libemk.a:	make.sh k/em_private.h k/em.c $(OBS)
		EMHOME=$(EMHOME); export EMHOME; sh make.sh k
		-sh -c 'ranlib libemk.a'

make.sh:	em.gen em.nogen make.fun
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
		rm -f C_*.c
		rm -f *.a em_code.3 em.gen make.sh

lintlib:	make.sh
		lint -I../../h -I../../../h -Ie -Ceme *.c e/*.c
		lint -I../../h -I../../../h -Ik -Cemk *.c k/*.c
		mv llib-leme.ln llib-lemk.ln $(EMHOME)/modules/lib

insert.o:	insert.c insert.h
		$(CC) $(CFLAGS) -c $(INCORE) insert.c

io.o:		io.c insert.h
		$(CC) $(CFLAGS) -c $(INCORE) io.c
