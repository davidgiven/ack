# make modula-2 "compiler"
# $Header$

HDIR =	../../em/h
PKGDIR =	../../em/pkg
LIBDIR =	../../em/lib

INCLUDES = -I$(HDIR) -I$(PKGDIR) -I/user1/erikb/em/h

LSRC =	tokenfile.g program.g declar.g expression.g statement.g
CC =	cc
GEN =	LLgen
GENOPTIONS =
PROFILE =
CFLAGS = $(PROFILE) $(INCLUDES)
LFLAGS = $(PROFILE)
LOBJ =	tokenfile.o program.o declar.o expression.o statement.o
COBJ =	LLlex.o LLmessage.o char.o error.o main.o \
	symbol2str.o tokenname.o idf.o input.o type.o def.o \
	scope.o misc.o enter.o defmodule.o typequiv.o node.o \
	cstoper.o chk_expr.o options.o walk.o
OBJ =	$(COBJ) $(LOBJ) Lpars.o
GENFILES=	tokenfile.c \
	program.c declar.c expression.c statement.c \
	tokenfile.g symbol2str.c char.c Lpars.c Lpars.h

all:
	make hfiles
	make LLfiles
	make main

LLfiles:	$(LSRC)
	$(GEN) $(GENOPTIONS) $(LSRC)
	@touch LLfiles

hfiles:	Parameters make.hfiles
	make.hfiles Parameters
	touch hfiles

main:	$(OBJ) Makefile
	$(CC) $(LFLAGS) $(OBJ) /user1/erikb/em/lib/libem_mes.a /user1/erikb/em/lib/libeme.a $(LIBDIR)/libcomp.a $(LIBDIR)/malloc.o /user1/erikb/em/lib/libprint.a /user1/erikb/em/lib/libstr.a /user1/erikb/em/lib/libsystem.a -o main
	size main

clean:
	rm -f $(OBJ) $(GENFILES) LLfiles 

lint:	LLfiles lintlist
	lint $(INCLUDES) `cat lintlist`

tokenfile.g:	tokenname.c make.tokfile
	make.tokfile <tokenname.c >tokenfile.g

symbol2str.c:	tokenname.c make.tokcase
	make.tokcase <tokenname.c >symbol2str.c

misc.h:		misc.H make.allocd
def.h:		def.H make.allocd
type.h:		type.H make.allocd
node.h:		node.H make.allocd
scope.c:	scope.C make.allocd

char.c: char.tab tab
	./tab -fchar.tab >char.c

tab: 
	$(CC) tab.c -o tab

depend:
	sed '/^#AUTOAUTO/,$$d' Makefile > Makefile.new
	echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
	/user1/erikb/bin/mkdep `sources $(OBJ)` |\
		sed 's/\.c:/\.o:/' >> Makefile.new
	mv Makefile Makefile.old
	mv Makefile.new Makefile

.SUFFIXES:	.H .h .C
.H.h .C.c :
	make.allocd < $< > $@

#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
LLlex.o: LLlex.h Lpars.h class.h const.h f_info.h idf.h idfsize.h input.h inputtype.h numsize.h strsize.h type.h
LLmessage.o: LLlex.h Lpars.h idf.h
char.o: class.h
error.o: LLlex.h debug.h errout.h f_info.h input.h inputtype.h main.h node.h
main.o: LLlex.h Lpars.h debug.h def.h f_info.h idf.h input.h inputtype.h ndirs.h node.h scope.h standards.h tokenname.h type.h
symbol2str.o: Lpars.h
tokenname.o: Lpars.h idf.h tokenname.h
idf.o: idf.h
input.o: f_info.h input.h inputtype.h
type.o: LLlex.h const.h debug.h def.h idf.h maxset.h node.h target_sizes.h type.h
def.o: LLlex.h debug.h def.h idf.h main.h node.h scope.h type.h
scope.o: LLlex.h debug.h def.h idf.h node.h scope.h type.h
misc.o: LLlex.h f_info.h idf.h misc.h node.h
enter.o: LLlex.h def.h idf.h main.h node.h scope.h type.h
defmodule.o: LLlex.h debug.h def.h f_info.h idf.h input.h inputtype.h main.h scope.h
typequiv.o: def.h type.h
node.o: LLlex.h debug.h def.h node.h type.h
cstoper.o: LLlex.h Lpars.h idf.h node.h standards.h target_sizes.h type.h
chk_expr.o: LLlex.h Lpars.h const.h debug.h def.h idf.h node.h scope.h standards.h type.h
options.o: idfsize.h main.h ndir.h type.h
walk.o: LLlex.h Lpars.h debug.h def.h main.h node.h scope.h type.h
tokenfile.o: Lpars.h
program.o: LLlex.h Lpars.h debug.h def.h idf.h main.h node.h scope.h type.h
declar.o: LLlex.h Lpars.h def.h idf.h main.h misc.h node.h scope.h type.h
expression.o: LLlex.h Lpars.h const.h debug.h def.h idf.h node.h scope.h type.h
statement.o: LLlex.h Lpars.h def.h idf.h node.h scope.h type.h
Lpars.o: Lpars.h
