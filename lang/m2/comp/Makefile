# make modula-2 "compiler"
EMDIR =		../../..
MHDIR =		$(EMDIR)/modules/h
PKGDIR =	$(EMDIR)/modules/pkg
LIBDIR =	$(EMDIR)/modules/lib
OBJECTCODE =	$(LIBDIR)/libemk.a
LLGEN =		$(EMDIR)/bin/LLgen

INCLUDES = -I$(MHDIR) -I$(EMDIR)/h -I$(PKGDIR)

GFILES =	tokenfile.g program.g declar.g expression.g statement.g
CC =	cc
LLGENOPTIONS =
PROFILE =
CFLAGS = $(PROFILE) $(INCLUDES) -DSTATIC=
LINTFLAGS = -DSTATIC= -DNORCSID
MALLOC = $(LIBDIR)/dickmalloc.o
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
	idfsize.h numsize.h strsize.h target_sizes.h \
	inputtype.h maxset.h ndir.h density.h\
	def.h debugcst.h type.h Lpars.h node.h
HFILES=		LLlex.h\
	chk_expr.h class.h const.h debug.h desig.h f_info.h idf.h\
	input.h main.h misc.h scope.h standards.h tokenname.h\
	walk.h warning.h $(GENHFILES)
#
GENFILES = $(GENGFILES) $(GENCFILES) $(GENHFILES)

#EXCLEXCLEXCLEXCL

all:	Cfiles
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make ../comp/main ; else sh Resolve ../comp/main ; fi'
	@rm -f nmclash.o a.out

install:	all
	cp main $(EMDIR)/lib/em_m2

clean:
	rm -f $(OBJ) $(GENFILES) LLfiles hfiles Cfiles tab clashes main
	(cd .. ; rm -rf Xsrc)

lint:	Cfiles
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make Xlint ; else sh Resolve Xlint ; fi'
	@rm -f nmclash.o a.out

longnames:	$(SRC) $(HFILES)
	sh -c 'if test -f longnames ; then prid -l7 longnames $? > Xlongnames ; mv Xlongnames longnames ; else prid -l7 $? > longnames ; fi'

# entry points not to be used directly

Cfiles:	hfiles LLfiles $(GENCFILES) $(GENHFILES) Makefile
	echo $(SRC) $(HFILES) > Cfiles

LLfiles:	$(GFILES)
	$(LLGEN) $(LLGENOPTIONS) $(GFILES)
	@touch LLfiles

hfiles:	Parameters make.hfiles
	make.hfiles Parameters
	touch hfiles

tokenfile.g:	tokenname.c make.tokfile
	make.tokfile <tokenname.c >tokenfile.g

symbol2str.c:	tokenname.c make.tokcase
	make.tokcase <tokenname.c >symbol2str.c

def.h:		def.H make.allocd
		make.allocd < def.H > def.h

type.h:		type.H make.allocd
		make.allocd < type.H > type.h

node.h:		node.H make.allocd
		make.allocd < node.H > node.h

scope.c:	scope.C make.allocd
		make.allocd < scope.C > scope.c

tmpvar.c:	tmpvar.C make.allocd
		make.allocd < tmpvar.C > tmpvar.c

casestat.c:	casestat.C make.allocd
		make.allocd < casestat.C > casestat.c

char.c: char.tab tab
	tab -fchar.tab >char.c

tab: 
	$(CC) tab.c -o tab

depend:
	sed '/^#AUTOAUTO/,$$d' Makefile > Makefile.new
	echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
	mkdep $(SRC) |\
		sed 's/\.c:/\.o:/' >> Makefile.new
	mv Makefile Makefile.old
	mv Makefile.new Makefile

#INCLINCLINCLINCL

Xlint:
	lint $(INCLUDES) $(LINTFLAGS) $(SRC)

../comp/main:	$(OBJ) ../comp/Makefile
	$(CC) $(LFLAGS) $(OBJ) $(LIBDIR)/libem_mes.a $(OBJECTCODE) $(LIBDIR)/libinput.a $(LIBDIR)/libassert.a $(LIBDIR)/liballoc.a $(MALLOC) $(LIBDIR)/libprint.a $(LIBDIR)/libstring.a $(LIBDIR)/libsystem.a -o ../comp/main
	size ../comp/main

#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
LLlex.o: LLlex.h Lpars.h class.h const.h debug.h debugcst.h f_info.h idf.h idfsize.h input.h inputtype.h numsize.h strsize.h type.h warning.h
LLmessage.o: LLlex.h Lpars.h idf.h
char.o: class.h
error.o: LLlex.h debug.h debugcst.h errout.h f_info.h input.h inputtype.h main.h node.h warning.h
main.o: LLlex.h Lpars.h debug.h debugcst.h def.h f_info.h idf.h input.h inputtype.h ndir.h node.h scope.h standards.h tokenname.h type.h warning.h
symbol2str.o: Lpars.h
tokenname.o: Lpars.h idf.h tokenname.h
idf.o: idf.h
input.o: def.h f_info.h idf.h input.h inputtype.h scope.h
type.o: LLlex.h const.h debug.h debugcst.h def.h idf.h maxset.h node.h scope.h target_sizes.h type.h walk.h
def.o: LLlex.h Lpars.h debug.h debugcst.h def.h idf.h main.h node.h scope.h type.h
scope.o: LLlex.h debug.h debugcst.h def.h idf.h node.h scope.h type.h
misc.o: LLlex.h f_info.h idf.h misc.h node.h
enter.o: LLlex.h debug.h debugcst.h def.h idf.h main.h node.h scope.h type.h
defmodule.o: LLlex.h Lpars.h debug.h debugcst.h def.h f_info.h idf.h input.h inputtype.h main.h node.h scope.h type.h
typequiv.o: LLlex.h debug.h debugcst.h def.h node.h type.h warning.h
node.o: LLlex.h debug.h debugcst.h def.h node.h type.h
cstoper.o: LLlex.h Lpars.h debug.h debugcst.h idf.h node.h standards.h target_sizes.h type.h warning.h
chk_expr.o: LLlex.h Lpars.h chk_expr.h const.h debug.h debugcst.h def.h idf.h misc.h node.h scope.h standards.h type.h warning.h
options.o: idfsize.h main.h ndir.h type.h warning.h
walk.o: LLlex.h Lpars.h chk_expr.h debug.h debugcst.h def.h desig.h f_info.h idf.h main.h node.h scope.h type.h walk.h warning.h
casestat.o: LLlex.h Lpars.h debug.h debugcst.h density.h desig.h node.h type.h walk.h
desig.o: LLlex.h debug.h debugcst.h def.h desig.h node.h scope.h type.h
code.o: LLlex.h Lpars.h debug.h debugcst.h def.h desig.h node.h scope.h standards.h type.h walk.h
tmpvar.o: debug.h debugcst.h def.h main.h scope.h type.h
lookup.o: LLlex.h debug.h debugcst.h def.h idf.h misc.h node.h scope.h type.h
tokenfile.o: Lpars.h
program.o: LLlex.h Lpars.h debug.h debugcst.h def.h f_info.h idf.h main.h node.h scope.h type.h warning.h
declar.o: LLlex.h Lpars.h chk_expr.h debug.h debugcst.h def.h idf.h main.h misc.h node.h scope.h type.h warning.h
expression.o: LLlex.h Lpars.h chk_expr.h const.h debug.h debugcst.h def.h idf.h node.h type.h warning.h
statement.o: LLlex.h Lpars.h def.h idf.h node.h scope.h type.h
Lpars.o: Lpars.h
