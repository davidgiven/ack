# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ack

HFILES=\
	$(SRC_DIR)/ack.h \
	$(SRC_DIR)/list.h \
	$(SRC_DIR)/trans.h \
	$(SRC_DIR)/data.h \
	$(SRC_DIR)/dmach.h \
	$(SRC_DIR)/grows.h

DSRC=\
	$(SRC_DIR)/list.c \
	$(SRC_DIR)/data.c \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/scan.c \
	$(SRC_DIR)/svars.c \
	$(SRC_DIR)/trans.c \
	$(SRC_DIR)/util.c \
	$(SRC_DIR)/rmach.c \
	$(SRC_DIR)/run.c \
	$(SRC_DIR)/grows.c \
	$(SRC_DIR)/files.c

ISRC=\
	dmach.c \
	intable.c

CFILES = $(DSRC) $(ISRC)

OBJ=\
	list.$(SUF) \
	data.$(SUF) \
	main.$(SUF) \
	scan.$(SUF) \
	svars.$(SUF) \
	trans.$(SUF) \
	util.$(SUF) \
	rmach.$(SUF) \
	run.$(SUF) \
	dmach.$(SUF) \
	intable.$(SUF) \
	grows.$(SUF) \
	files.$(SUF)

ACKDIR=$(TARGET_HOME)/lib
FE=fe
INTABLES=sun3
LNTABLES=acc apc abc ocm m2 vax4 i86 i386 m68k2 m68k4 pmds pmds4 mantra \
	m68020 z8000 pdp em22 em24 em44 6500 6800 6805 6809 i80 ns s2650 z80 \
	sun2 xenix3 minix minixST
INCLUDES=-I$(TARGET_HOME)/h -I$(TARGET_HOME)/config -I$(SRC_DIR)
CFLAGS= $(INCLUDES) $(COPTIONS)
UCFLAGS= $(INCLUDES) $(UCOPTIONS)
LINTFLAGS= $(INCLUDES) $(LINTOPTIONS)
LDFLAGS= $(LDOPTIONS)
ULDFLAGS= $(ULDOPTIONS)
BINDIR=$(TARGET_HOME)/bin
HDIR=$(TARGET_HOME)/h
MANDIR=$(TARGET_HOME)/man
MODDIR=$(TARGET_HOME)/modules/lib

all:		ack ack.1

install:	ack ack.1
		rm -f $(BINDIR)/ack
		cp ack $(BINDIR)/ack
		-cd $(BINDIR) ; \
		for i in $(INTABLES) $(LNTABLES) ; do rm -f $$i ; ln ack $$i ; done
		rm -f $(MANDIR)/ack.1
		cp ack.1 $(MANDIR)/ack.1

cmp:		ack ack.1
		-cmp ack $(BINDIR)/ack
		-cmp ack.1 $(MANDIR)/ack.1

ack.1:		$(SRC_DIR)/ack.1.X
		-sh -c 'tbl < $(SRC_DIR)/ack.1.X > ack.1'
		-sh -c 'if test -s ack.1 ; then : ; else cp $(SRC_DIR)/ack.1.X ack.1 ; fi'

clean:
		-rm -f *.old *.$(SUF) mktables *.$(USUF) ack ack.1 $(ISRC)

ack:    $(OBJ)
	$(CC) $(LDFLAGS) -o ack $(OBJ) $(MODDIR)/libstring.$(LIBSUF)

depend:	$(ISRC)
	sed '/^#DEPENDENCIES/,$$d' Makefile >Makefile.new
	echo '#DEPENDENCIES' >>Makefile.new
	for i in $(CFILES) ; do \
		echo "`basename $$i .c`.$$(SUF):	$$i" >> Makefile.new ; \
		echo '	$$(CC) -c $$(CFLAGS)' $$i >> Makefile.new ; \
		$(UTIL_HOME)/lib.bin/cpp -d $(INCLUDES) $$i | sed "s/^/`basename $$i .c`.$$(SUF):	/" >> Makefile.new ; \
	done
	mv Makefile Makefile.old
	mv Makefile.new Makefile

dmach.c intable.c: mktables $(SRC_DIR)/dmach.h
	: mktables $(ACKDIR) # $(FE) $(INTABLES)
	mktables $(ACKDIR)

mktables:	mktables.$(USUF)
	$(UCC) -o mktables $(ULDFLAGS) mktables.$(USUF)

mktables.$(USUF):	$(SRC_DIR)/mktables.c
	$(UCC) -c $(UCFLAGS) $(SRC_DIR)/mktables.c

pr:
	@pr $(SRC_DIR)/proto.make $(HFILES) $(DSRC)

opr:
	make pr | opr

lint:   $(ISRC)
	$(LINT) $(LINTFLAGS) $(CFILES) $(UTIL_HOME)/modules/lib/$(LINTPREF)string.$(LINTSUF)

# do not remove the next line
#DEPENDENCIES
list.$(SUF):	$(SRC_DIR)/list.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/list.c
list.$(SUF):	$(SRC_DIR)/list.h
list.$(SUF):	$(SRC_DIR)/ack.h
data.$(SUF):	$(SRC_DIR)/data.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/data.c
data.$(SUF):	$(SRC_DIR)/data.h
data.$(SUF):	$(SRC_DIR)/trans.h
data.$(SUF):	$(SRC_DIR)/list.h
data.$(SUF):	$(SRC_DIR)/ack.h
main.$(SUF):	$(SRC_DIR)/main.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/main.c
main.$(SUF):	$(SRC_DIR)/data.h
main.$(SUF):	$(SRC_DIR)/config/local.h
main.$(SUF):	$(SRC_DIR)/trans.h
main.$(SUF):	$(SRC_DIR)/list.h
main.$(SUF):	$(SRC_DIR)/ack.h
scan.$(SUF):	$(SRC_DIR)/scan.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/scan.c
scan.$(SUF):	$(SRC_DIR)/data.h
scan.$(SUF):	$(SRC_DIR)/trans.h
scan.$(SUF):	$(SRC_DIR)/list.h
scan.$(SUF):	$(SRC_DIR)/ack.h
svars.$(SUF):	$(SRC_DIR)/svars.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/svars.c
svars.$(SUF):	$(SRC_DIR)/ack.h
trans.$(SUF):	$(SRC_DIR)/trans.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/trans.c
trans.$(SUF):	$(SRC_DIR)/data.h
trans.$(SUF):	$(SRC_DIR)/grows.h
trans.$(SUF):	$(SRC_DIR)/trans.h
trans.$(SUF):	$(SRC_DIR)/list.h
trans.$(SUF):	$(SRC_DIR)/ack.h
util.$(SUF):	$(SRC_DIR)/util.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/util.c
util.$(SUF):	$(SRC_DIR)/ack.h
rmach.$(SUF):	$(SRC_DIR)/rmach.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/rmach.c
rmach.$(SUF):	$(SRC_DIR)/data.h
rmach.$(SUF):	$(SRC_DIR)/dmach.h
rmach.$(SUF):	$(SRC_DIR)/grows.h
rmach.$(SUF):	$(SRC_DIR)/trans.h
rmach.$(SUF):	$(SRC_DIR)/list.h
rmach.$(SUF):	$(HDIR)/em_path.h
rmach.$(SUF):	$(SRC_DIR)/ack.h
run.$(SUF):	$(SRC_DIR)/run.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/run.c
run.$(SUF):	$(SRC_DIR)/data.h
run.$(SUF):	$(SRC_DIR)/grows.h
run.$(SUF):	$(SRC_DIR)/trans.h
run.$(SUF):	$(SRC_DIR)/list.h
run.$(SUF):	$(SRC_DIR)/ack.h
grows.$(SUF):	$(SRC_DIR)/grows.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/grows.c
grows.$(SUF):	$(SRC_DIR)/grows.h
grows.$(SUF):	$(SRC_DIR)/ack.h
files.$(SUF):	$(SRC_DIR)/files.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/files.c
files.$(SUF):	$(HDIR)/em_path.h
files.$(SUF):	$(SRC_DIR)/data.h
files.$(SUF):	$(SRC_DIR)/grows.h
files.$(SUF):	$(SRC_DIR)/trans.h
files.$(SUF):	$(SRC_DIR)/list.h
files.$(SUF):	$(SRC_DIR)/ack.h
dmach.$(SUF):	dmach.c
	$(CC) -c $(CFLAGS) dmach.c
dmach.$(SUF):	$(SRC_DIR)/dmach.h
intable.$(SUF):	intable.c
	$(CC) -c $(CFLAGS) intable.c
