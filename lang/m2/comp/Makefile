# make modula-2 "compiler"
EMDIR =		/usr/ceriel/em
MHDIR =		$(EMDIR)/modules/h
PKGDIR =	$(EMDIR)/modules/pkg
LIBDIR =	$(EMDIR)/modules/lib
LLGEN =		$(EMDIR)/bin/LLgen

INCLUDES = -I$(MHDIR) -I$(EMDIR)/h -I$(PKGDIR)

GFILES =	tokenfile.g program.g declar.g expression.g statement.g
CC =	cc
LLGENOPTIONS =
PROFILE =
CFLAGS = $(PROFILE) $(INCLUDES) -DSTATIC=
LINTFLAGS = -DSTATIC= -DNORCSID
LFLAGS = $(PROFILE)
LSRC =	tokenfile.c program.c declar.c expression.c statement.c
LOBJ =	tokenfile.o program.o declar.o expression.o statement.o
CSRC =	LLlex.c LLmessage.c char.c error.c main.c \
	symbol2str.c tokenname.c idf.c input.c type.c def.c \
	scope.c misc.c enter.c defmodule.c typequiv.c node.c \
	cstoper.c chk_expr.c options.c walk.c casestat.c desig.c \
	code.c tmpvar.c lookup.c Version.c
COBJ =	LLlex.o LLmessage.o char.o error.o main.o \
	symbol2str.o tokenname.o idf.o input.o type.o def.o \
	scope.o misc.o enter.o defmodule.o typequiv.o node.o \
	cstoper.o chk_expr.o options.o walk.o casestat.o desig.o \
	code.o tmpvar.o lookup.o Version.o
SRC =	$(CSRC) $(LSRC) Lpars.c
OBJ =	$(COBJ) $(LOBJ) Lpars.o

# Keep the next entries up to date!
GENCFILES=	tokenfile.c \
	program.c declar.c expression.c statement.c \
	symbol2str.c char.c Lpars.c casestat.c tmpvar.c scope.c
GENGFILES=	tokenfile.g
GENHFILES=	errout.h\
	idfsize.h numsize.h strsize.h target_sizes.h debug.h\
	inputtype.h maxset.h ndir.h density.h\
	def.h type.h Lpars.h node.h
HFILES=		LLlex.h\
	chk_expr.h class.h const.h desig.h f_info.h idf.h\
	input.h main.h misc.h scope.h standards.h tokenname.h\
	walk.h $(GENHFILES)
#
GENFILES = $(GENGFILES) $(GENCFILES) $(GENHFILES)

all:	Cfiles
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make main ; else sh Resolve main ; fi'
	@rm -f nmclash.o a.out

clean:
	rm -f $(OBJ) $(GENFILES) LLfiles hfiles Cfiles tab clashes
	(cd .. ; rm -rf Xsrc)

lint:	Cfiles
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make Xlint ; else sh Resolve Xlint ; fi'
	@rm -f nmclash.o a.out

clashes:	$(SRC) $(HFILES)
	sh -c 'if test -f clashes ; then cclash -l7 clashes $? > Xclashes ; mv Xclashes clashes ; else cclash -l7 $? > clashes ; fi'

# entry points not to be used directly

Xlint:
	lint $(INCLUDES) $(LINTFLAGS) $(SRC)

Cfiles:	hfiles LLfiles $(GENCFILES) $(GENHFILES)
	echo $(SRC) $(HFILES) > Cfiles

LLfiles:	$(GFILES)
	$(LLGEN) $(LLGENOPTIONS) $(GFILES)
	@touch LLfiles

hfiles:	Parameters make.hfiles
	make.hfiles Parameters
	touch hfiles

main:	$(OBJ) ../src/Makefile
	$(CC) $(LFLAGS) $(OBJ) $(LIBDIR)/libem_mes.a $(LIBDIR)/libemk.a $(LIBDIR)/input.a $(LIBDIR)/assert.a $(LIBDIR)/alloc.a $(LIBDIR)/dickmalloc.o $(LIBDIR)/libprint.a $(LIBDIR)/libstr.a $(LIBDIR)/libsystem.a -o ../src/main
	size ../src/main

tokenfile.g:	tokenname.c make.tokfile
	make.tokfile <tokenname.c >tokenfile.g

symbol2str.c:	../src/tokenname.c ../src/make.tokcase
	../src/make.tokcase <../src/tokenname.c >symbol2str.c

def.h:		../src/def.H ../src/make.allocd
		../src/make.allocd < ../src/def.H > def.h

type.h:		../src/type.H ../src/make.allocd
		../src/make.allocd < ../src/type.H > type.h

node.h:		../src/node.H ../src/make.allocd
		../src/make.allocd < ../src/node.H > node.h

scope.c:	../src/scope.C ../src/make.allocd
		../src/make.allocd < ../src/scope.C > scope.c

tmpvar.c:	../src/tmpvar.C ../src/make.allocd
		../src/make.allocd < ../src/tmpvar.C > tmpvar.c

casestat.c:	../src/casestat.C ../src/make.allocd
		../src/make.allocd < ../src/casestat.C > casestat.c

char.c: ../src/char.tab ../src/tab
	../src/tab -fchar.tab >char.c

../src/tab: 
	$(CC) ../src/tab.c -o ../src/tab

depend:
	sed '/^#AUTOAUTO/,$$d' Makefile > Makefile.new
	echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
	mkdep $(SRC) |\
		sed 's/\.c:/\.o:/' >> Makefile.new
	mv Makefile Makefile.old
	mv Makefile.new Makefile

#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
LLlex.o: LLlex.h Lpars.h class.h const.h debug.h f_info.h idf.h idfsize.h input.h inputtype.h numsize.h strsize.h type.h
LLmessage.o: LLlex.h Lpars.h idf.h
char.o: class.h
error.o: LLlex.h debug.h errout.h f_info.h input.h inputtype.h main.h node.h
main.o: LLlex.h Lpars.h debug.h def.h f_info.h idf.h input.h inputtype.h ndir.h node.h scope.h standards.h tokenname.h type.h
symbol2str.o: Lpars.h
tokenname.o: Lpars.h idf.h tokenname.h
idf.o: idf.h
input.o: def.h f_info.h idf.h input.h inputtype.h scope.h
type.o: LLlex.h const.h debug.h def.h idf.h maxset.h node.h scope.h target_sizes.h type.h walk.h
def.o: LLlex.h Lpars.h debug.h def.h idf.h main.h node.h scope.h type.h
scope.o: LLlex.h debug.h def.h idf.h node.h scope.h type.h
misc.o: LLlex.h f_info.h idf.h misc.h node.h
enter.o: LLlex.h debug.h def.h idf.h main.h node.h scope.h type.h
defmodule.o: LLlex.h Lpars.h debug.h def.h f_info.h idf.h input.h inputtype.h main.h node.h scope.h
typequiv.o: LLlex.h debug.h def.h node.h type.h
node.o: LLlex.h debug.h def.h node.h type.h
cstoper.o: LLlex.h Lpars.h debug.h idf.h node.h standards.h target_sizes.h type.h
chk_expr.o: LLlex.h Lpars.h chk_expr.h const.h debug.h def.h idf.h node.h scope.h standards.h type.h
options.o: idfsize.h main.h ndir.h type.h
walk.o: LLlex.h Lpars.h chk_expr.h debug.h def.h desig.h f_info.h idf.h main.h node.h scope.h type.h walk.h
casestat.o: LLlex.h Lpars.h debug.h density.h desig.h node.h type.h walk.h
desig.o: LLlex.h debug.h def.h desig.h node.h scope.h type.h
code.o: LLlex.h Lpars.h debug.h def.h desig.h node.h scope.h standards.h type.h walk.h
tmpvar.o: debug.h def.h main.h scope.h type.h
lookup.o: LLlex.h debug.h def.h idf.h node.h scope.h type.h
tokenfile.o: Lpars.h
program.o: LLlex.h Lpars.h debug.h def.h f_info.h idf.h main.h node.h scope.h type.h
declar.o: LLlex.h Lpars.h chk_expr.h debug.h def.h idf.h main.h misc.h node.h scope.h type.h
expression.o: LLlex.h Lpars.h chk_expr.h const.h debug.h def.h idf.h node.h type.h
statement.o: LLlex.h Lpars.h def.h idf.h node.h scope.h type.h
Lpars.o: Lpars.h
