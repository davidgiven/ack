EMHOME = ../../..
ETC = $(EMHOME)/etc
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare
CFLAGS = -I$(EMHOME)/h -I$(EMHOME)/modules/h -I. -O
AR = ar
SRC =	bhcst.c bhdlb.c bhdnam.c bhfcon.c bhicon.c bhilb.c bhpnam.c bhucon.c \
	crcst.c crdlb.c crdnam.c crfcon.c cricon.c crilb.c crpnam.c crscon.c \
	crucon.c cst.c dfdlb.c dfdnam.c dfilb.c dlb.c dnam.c end.c endarg.c \
	exc.c failed.c fcon.c getid.c icon.c ilb.c insert.c internerr.c io.c \
	msend.c op.c opcst.c opdlb.c opdnam.c opilb.c opnarg.c oppnam.c pnam.c \
	pro.c pronarg.c msstart.c psdlb.c psdnam.c pspnam.c scon.c ucon.c \
	C_out.c

OBS = failed.o insert.o internerr.o getid.o

all:		libeme.a libemk.a em_code.3 em_codeEK.h
		rm -f C_*.c

install:	all
		$(INSTALL) lib/libeme.a
		$(INSTALL) lib/libemk.a
		$(INSTALL) man/em_code.3
		$(INSTALL) h/em_codeEK.h

compare:	all
		$(COMPARE) lib/libeme.a
		$(COMPARE) lib/libemk.a
		$(COMPARE) man/em_code.3
		$(COMPARE) h/em_codeEK.h

em_code.3:	em_code.3X
		-sh -c 'tbl < em_code.3X > em_code.3'
		-sh -c 'if test -s em_code.3 ; then : ; else cp em_code.3X em_code.3 ; fi '

libeme.a:	e/em_private.h e/em.c $(OBS) io.c
		EMHOME=$(EMHOME); cc="$(CC)"; cflags="-c -Ie $(CFLAGS)" ar="$(AR)"; export EMHOME cc ar cflags; sh make.sh e
		-sh -c 'ranlib libeme.a'

libemk.a:	k/em_private.h k/em.c $(OBS) io.c
		EMHOME=$(EMHOME); cc="$(CC)"; cflags="-c -Ik $(CFLAGS)" ar="$(AR)"; export EMHOME cc ar cflags; sh make.sh k
		-sh -c 'ranlib libemk.a'

em_codeEK.h:	make.em.gen $(ETC)/em_table em.nogen
		make.em.gen $(ETC)/em_table > em_codeEK.h
		cat em.nogen >> em_codeEK.h

pr:
		@pr Makefile em.nogen make.em.gen make.sh insert.h $(SRC) e/em_private.h e/em.c k/em_private.h k/em.c

opr:
		make pr | opr

# don't put the next "rm"s all on one line. the argument list then
# becomes too long for some systems
clean:
		rm -f *.o *.a em_code.3

lintlib:	make.sh
		lint -I. -I../../h -I../../../h -Ie -Ceme $(SRC) e/*.c
		lint -I. -I../../h -I../../../h -Ik -Cemk $(SRC) k/*.c
		mv llib-leme.ln llib-lemk.ln $(EMHOME)/modules/lib

insert.o:	insert.c insert.h
		$(CC) $(CFLAGS) -c insert.c
