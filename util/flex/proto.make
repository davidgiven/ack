# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/flex

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
# AUXDIR;
DESTDIR =
BINDIR = $(TARGET_HOME)/bin
AUXDIR = $(TARGET_HOME)/lib/flex

# MAKE = make

SKELETON_FILE = \"$(AUXDIR)/flex.skel\"
SKELFLAGS = -DDEFAULT_SKELETON_FILE=$(SKELETON_FILE)
INCLUDES = -I$(SRC_DIR) -I.
CFLAGS = $(COPTIONS) -DACK_MOD $(INCLUDES)
LDFLAGS = $(LDOPTIONS)

COMPRESSION =
FLEX_FLAGS = -ist8 -Sflex.skel
# which "flex" to use to generate scan.c from scan.l
FLEX = flex

FLEXOBJS = \
	ccl.$(SUF) \
	dfa.$(SUF) \
	ecs.$(SUF) \
	gen.$(SUF) \
	main.$(SUF) \
	misc.$(SUF) \
	nfa.$(SUF) \
	parse.$(SUF) \
	scan.$(SUF) \
	sym.$(SUF) \
	tblcmp.$(SUF) \
	yylex.$(SUF)

FLEX_C_SOURCES = \
	$(SRC_DIR)/ccl.c \
	$(SRC_DIR)/dfa.c \
	$(SRC_DIR)/ecs.c \
	$(SRC_DIR)/gen.c \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/misc.c \
	$(SRC_DIR)/nfa.c \
	$(SRC_DIR)/parse.c \
	$(SRC_DIR)/scan.c \
	$(SRC_DIR)/sym.c \
	$(SRC_DIR)/tblcmp.c \
	$(SRC_DIR)/yylex.c


all : flex

flex : $(FLEXOBJS)
	$(CC) -o flex $(LDFLAGS) $(FLEXOBJS)

first_flex:
	cp $(SRC_DIR)/initscan.c scan.c
	chmod +w scan.c
	make $(MFLAGS) flex

parse.h parse.c : $(SRC_DIR)/parse.y
	yacc -d $(SRC_DIR)/parse.y
	@mv y.tab.c parse.c
	@mv y.tab.h parse.h

scan.c : $(SRC_DIR)/scan.l
	$(FLEX) $(FLEX_FLAGS) $(COMPRESSION) $(SRC_DIR)/scan.l >scan.c

scan.$(SUF) : scan.c parse.h $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c scan.c

main.$(SUF) : $(SRC_DIR)/main.c $(SRC_DIR)/flexdef.h
	-mkdir $(AUXDIR)
	$(CC) $(CFLAGS) -c $(SKELFLAGS) $(SRC_DIR)/main.c

ccl.$(SUF) : $(SRC_DIR)/ccl.c $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/ccl.c
dfa.$(SUF) : $(SRC_DIR)/dfa.c $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/dfa.c
ecs.$(SUF) : $(SRC_DIR)/ecs.c $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/ecs.c
gen.$(SUF) : $(SRC_DIR)/gen.c $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/gen.c
misc.$(SUF) : $(SRC_DIR)/misc.c $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/misc.c
nfa.$(SUF) : $(SRC_DIR)/nfa.c $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/nfa.c
parse.$(SUF) : parse.c $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c parse.c
sym.$(SUF) : $(SRC_DIR)/sym.c $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/sym.c
tblcmp.$(SUF) : $(SRC_DIR)/tblcmp.c $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/tblcmp.c
yylex.$(SUF) : $(SRC_DIR)/yylex.c $(SRC_DIR)/flexdef.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/yylex.c

LINTFLAGS = $(LINTOPTIONS) $(INCLUDES)
lint : $(FLEX_C_SOURCES)
	$(LINT) $(LINTFLAGS) $(FLEX_C_SOURCES)

firstinstall: first_flex $(SRC_DIR)/flex.skel
	rm -f $(BINDIR)/flex
	cp flex $(BINDIR)/flex
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	cp $(SRC_DIR)/flex.skel $(AUXDIR)/flex.skel ; \
		mk_manpage $(SRC_DIR)/flex.1 $(TARGET_HOME) ; \
		mk_manpage $(SRC_DIR)/flexdoc.1 $(TARGET_HOME) ; \
	fi

install: flex $(SRC_DIR)/flex.skel
	rm -f $(BINDIR)/flex
	cp flex $(BINDIR)/flex
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	cp $(SRC_DIR)/flex.skel $(AUXDIR)/flex.skel ; \
		mk_manpage $(SRC_DIR)/flex.1 $(TARGET_HOME) ; \
		mk_manpage $(SRC_DIR)/flexdoc.1 $(TARGET_HOME) ; \
	fi

cmp: flex $(SRC_DIR)/flex.skel
	-cmp flex $(BINDIR)/flex
	-cmp $(SRC_DIR)/flex.skel $(AUXDIR)/flex.skel 

clean :
	rm -f core errs flex *.$(SUF) parse.c *.lint parse.h tags scan.c

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
