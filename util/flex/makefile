# Note: this is a modified version of Makefile, for ACK installation. The
# original flex makefile has a capital M (Makefile).

EMHOME = ../..

# make file for "flex" tool

# Porting considerations:
#
#    For System V Unix machines, add -DUSG to CFLAGS (if it's not
#         automatically defined)
#    For Vax/VMS, add "-DVMS -DUSG" to CFLAGS.
#    For MS-DOS, add "-DMS_DOS -DUSG" to CFLAGS.  Create \tmp if not present.
#         You will also want to rename flex.skel to something with a three
#         character extension, change SKELETON_FILE below appropriately,
#	  See MSDOS.notes for more info.
#    For Amiga, add "-DAMIGA -DUSG" to CFLAGS.
#    For SCO Unix, add "-DSCO_UNIX" to CFLAGS.
#
#    For C compilers which don't know about "void", add -Dvoid=int to CFLAGS.
#
#    If your C compiler is ANSI standard but does not include the <stdlib.h>
#    header file (some installations of gcc have this problem), then add
#    -DDONT_HAVE_STDLIB_H to CFLAGS.
#
# By default, flex will be configured to generate 8-bit scanners only
# if the -8 flag is given.  If you want it to always generate 8-bit
# scanners, add "-DDEFAULT_CSIZE=256" to CFLAGS.  Note that doing
# so will double the size of all uncompressed scanners.
# 
# If on your system you have trouble building flex due to 8-bit
# character problems, remove the -8 from FLEX_FLAGS and the
# "#define FLEX_8_BIT_CHARS" from the beginning of flexdef.h.


# the first time around use "make first_flex"


# Installation targeting.  Files will be installed under the tree rooted
# at DESTDIR.  User commands will be installed in BINDIR, library files
# in LIBDIR (which will be created if necessary), auxiliary files in
# AUXDIR, manual pages will be installed in MANDIR with extension MANEXT.
# Raw, unformatted troff source will be installed if INSTALLMAN=man, nroff
# preformatted versions will be installed if INSTALLMAN=cat.
DESTDIR =
BINDIR = $(EMHOME)/bin
AUXDIR = $(EMHOME)/lib/flex
MANDIR = $(EMHOME)/man
MANEXT = 1
INSTALLMAN = man

# MAKE = make

SKELETON_FILE = \"`cd $(AUXDIR); pwd`/flex.skel\"
SKELFLAGS = -DDEFAULT_SKELETON_FILE=$(SKELETON_FILE)
CFLAGS = -O -Dvoid=int -DACK_MOD
LDFLAGS = -s

COMPRESSION =
FLEX_FLAGS = -ist8 -Sflex.skel
# which "flex" to use to generate scan.c from scan.l
FLEX = ./flex
# CC = cc

AR = ar
RANLIB = ranlib

FLEXOBJS = \
	ccl.o \
	dfa.o \
	ecs.o \
	gen.o \
	main.o \
	misc.o \
	nfa.o \
	parse.o \
	scan.o \
	sym.o \
	tblcmp.o \
	yylex.o

FLEX_C_SOURCES = \
	ccl.c \
	dfa.c \
	ecs.c \
	gen.c \
	main.c \
	misc.c \
	nfa.c \
	parse.c \
	scan.c \
	sym.c \
	tblcmp.c \
	yylex.c


all : flex

flex : $(FLEXOBJS)
	$(CC) $(CFLAGS) -o flex $(LDFLAGS) $(FLEXOBJS)

first_flex:
	cp initscan.c scan.c
	$(MAKE) $(MFLAGS) flex

parse.h parse.c : parse.y
	$(YACC) -d parse.y
	@mv y.tab.c parse.c
	@mv y.tab.h parse.h

scan.c : scan.l
	$(FLEX) $(FLEX_FLAGS) $(COMPRESSION) scan.l >scan.c

scan.o : scan.c parse.h flexdef.h

main.o : main.c flexdef.h
	-mkdir $(AUXDIR)
	$(CC) $(CFLAGS) -c $(SKELFLAGS) main.c

ccl.o : ccl.c flexdef.h
dfa.o : dfa.c flexdef.h
ecs.o : ecs.c flexdef.h
gen.o : gen.c flexdef.h
misc.o : misc.c flexdef.h
nfa.o : nfa.c flexdef.h
parse.o : parse.c flexdef.h
sym.o : sym.c flexdef.h
tblcmp.o : tblcmp.c flexdef.h
yylex.o : yylex.c flexdef.h

lint : $(FLEX_C_SOURCES)
	lint $(FLEX_C_SOURCES) > flex.lint

install: first_flex flex.skel
	rm -f $(BINDIR)flex
	cp flex $(BINDIR)/flex
	cp flex.skel $(AUXDIR)/flex.skel
	cp flex.1 $(MANDIR)/flex.1
	cp flexdoc.1 $(MANDIR)/flexdoc.1
	
clean :
	rm -f core errs flex *.o parse.c *.lint parse.h tags

tags :
	ctags $(FLEX_C_SOURCES)

test : flex
	./flex $(FLEX_FLAGS) $(COMPRESSION) scan.l | diff scan.c -

bigtest :
	rm -f scan.c ; $(MAKE) COMPRESSION="-C" test
	rm -f scan.c ; $(MAKE) COMPRESSION="-Ce" test
	rm -f scan.c ; $(MAKE) COMPRESSION="-Cm" test
	rm -f scan.c ; $(MAKE) COMPRESSION="-Cfe" test
	rm -f scan.c ; $(MAKE) COMPRESSION="-CFe" test
	rm -f scan.c ; $(MAKE) COMPRESSION="-Cf" test
	rm -f scan.c ; $(MAKE) COMPRESSION="-CF" test
	rm -f scan.c ; $(MAKE)
