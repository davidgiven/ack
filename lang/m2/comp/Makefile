# make modula-2 "compiler"
EMHOME =		../../..
MHDIR =		$(EMHOME)/modules/h
PKGDIR =	$(EMHOME)/modules/pkg
LIBDIR =	$(EMHOME)/modules/lib
OBJECTCODE =	$(LIBDIR)/libemk.a
LLGEN =		$(EMHOME)/bin/LLgen
MKDEP =		$(EMHOME)/bin/mkdep
PRID =		$(EMHOME)/bin/prid
CID =		$(EMHOME)/bin/cid
CURRDIR =	.

INCLUDES = -I$(MHDIR) -I$(EMHOME)/h -I$(PKGDIR)

GFILES =	tokenfile.g program.g declar.g expression.g statement.g
LLGENOPTIONS = -v
PROFILE =
CFLAGS = $(PROFILE) $(INCLUDES) -O -DSTATIC=
LINTFLAGS = -DSTATIC= -DNORCSID
MALLOC = $(LIBDIR)/malloc.o
LFLAGS = $(PROFILE)
LSRC =	tokenfile.c program.c declar.c expression.c statement.c
LOBJ =	tokenfile.o program.o declar.o expression.o statement.o
CSRC =	LLlex.c LLmessage.c char.c error.c main.c \
	symbol2str.c tokenname.c idf.c input.c type.c def.c \
	scope.c misc.c enter.c defmodule.c typequiv.c node.c \
	cstoper.c chk_expr.c options.c walk.c casestat.c desig.c \
	code.c tmpvar.c lookup.c Version.c next.c
COBJ =	LLlex.o LLmessage.o char.o error.o main.o \
	symbol2str.o tokenname.o idf.o input.o type.o def.o \
	scope.o misc.o enter.o defmodule.o typequiv.o node.o \
	cstoper.o chk_expr.o options.o walk.o casestat.o desig.o \
	code.o tmpvar.o lookup.o Version.o next.o
SRC =	$(CSRC) $(LSRC) Lpars.c
OBJ =	$(COBJ) $(LOBJ) Lpars.o

# Keep the next entries up to date!
GENCFILES=	tokenfile.c \
	program.c declar.c expression.c statement.c \
	symbol2str.c char.c Lpars.c casestat.c tmpvar.c scope.c next.c
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
NEXTFILES = def.H type.H node.H scope.C tmpvar.C casestat.C

#EXCLEXCLEXCLEXCL

all:	Cfiles
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make $(CURRDIR)/main ; else sh Resolve main ; fi'
	@rm -f nmclash.o a.out

install:	all
	cp $(CURRDIR)/main $(EMHOME)/lib/em_m2

clean:
	rm -f $(OBJ) $(GENFILES) LLfiles hfiles Cfiles tab clashes \
		$(CURRDIR)/main LL.output
	(cd .. ; rm -rf Xsrc)

lint:	Cfiles
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make Xlint ; else sh Resolve Xlint ; fi'
	@rm -f nmclash.o a.out

longnames:	$(SRC) $(HFILES)
	sh -c 'if test -f longnames ; then $(PRID) -l7 longnames $? > Xlongnames ; mv Xlongnames longnames ; else $(PRID) -l7 $? > longnames ; fi'

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

.SUFFIXES:	.H .h
.H.h:
		./make.allocd < $*.H > $*.h

.SUFFIXES:	.C .c
.C.c:
		./make.allocd < $*.C > $*.c

def.h:		make.allocd
type.h:		make.allocd
node.h:		make.allocd
scope.c:	make.allocd
tmpvar.c:	make.allocd
casestat.c:	make.allocd

next.c:		$(NEXTFILES) ./make.next
		./make.next $(NEXTFILES) > next.c

char.c: char.tab tab
	tab -fchar.tab >char.c

tab: 
	$(CC) tab.c -o tab

depend:
	sed '/^#AUTOAUTO/,$$d' Makefile > Makefile.new
	echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
	$(MKDEP) $(SRC) |\
		sed 's/\.c:/\.o:/' >> Makefile.new
	mv Makefile Makefile.old
	mv Makefile.new Makefile

#INCLINCLINCLINCL

Xlint:
	lint $(INCLUDES) $(LINTFLAGS) $(SRC)

$(CURRDIR)/main:	$(OBJ)
	$(CC) $(LFLAGS) $(OBJ) $(LIBDIR)/libem_mes.a $(OBJECTCODE) $(LIBDIR)/libinput.a $(LIBDIR)/libassert.a $(LIBDIR)/liballoc.a $(MALLOC) $(LIBDIR)/libprint.a $(LIBDIR)/libstring.a $(LIBDIR)/libsystem.a -o $(CURRDIR)/main
	size $(CURRDIR)/main

#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
LLlex.o: LLlex.h
LLlex.o: Lpars.h
LLlex.o: class.h
LLlex.o: const.h
LLlex.o: debug.h
LLlex.o: debugcst.h
LLlex.o: f_info.h
LLlex.o: idf.h
LLlex.o: idfsize.h
LLlex.o: input.h
LLlex.o: inputtype.h
LLlex.o: numsize.h
LLlex.o: strsize.h
LLlex.o: type.h
LLlex.o: warning.h
LLmessage.o: LLlex.h
LLmessage.o: Lpars.h
LLmessage.o: idf.h
char.o: class.h
error.o: LLlex.h
error.o: debug.h
error.o: debugcst.h
error.o: errout.h
error.o: f_info.h
error.o: input.h
error.o: inputtype.h
error.o: main.h
error.o: node.h
error.o: warning.h
main.o: LLlex.h
main.o: Lpars.h
main.o: debug.h
main.o: debugcst.h
main.o: def.h
main.o: f_info.h
main.o: idf.h
main.o: input.h
main.o: inputtype.h
main.o: ndir.h
main.o: node.h
main.o: scope.h
main.o: standards.h
main.o: tokenname.h
main.o: type.h
main.o: warning.h
symbol2str.o: Lpars.h
tokenname.o: Lpars.h
tokenname.o: idf.h
tokenname.o: tokenname.h
idf.o: idf.h
input.o: def.h
input.o: f_info.h
input.o: idf.h
input.o: input.h
input.o: inputtype.h
input.o: scope.h
type.o: LLlex.h
type.o: chk_expr.h
type.o: const.h
type.o: debug.h
type.o: debugcst.h
type.o: def.h
type.o: idf.h
type.o: maxset.h
type.o: node.h
type.o: scope.h
type.o: target_sizes.h
type.o: type.h
type.o: walk.h
def.o: LLlex.h
def.o: Lpars.h
def.o: debug.h
def.o: debugcst.h
def.o: def.h
def.o: idf.h
def.o: main.h
def.o: node.h
def.o: scope.h
def.o: type.h
scope.o: LLlex.h
scope.o: debug.h
scope.o: debugcst.h
scope.o: def.h
scope.o: idf.h
scope.o: node.h
scope.o: scope.h
scope.o: type.h
misc.o: LLlex.h
misc.o: f_info.h
misc.o: idf.h
misc.o: misc.h
misc.o: node.h
enter.o: LLlex.h
enter.o: debug.h
enter.o: debugcst.h
enter.o: def.h
enter.o: idf.h
enter.o: main.h
enter.o: misc.h
enter.o: node.h
enter.o: scope.h
enter.o: type.h
defmodule.o: LLlex.h
defmodule.o: Lpars.h
defmodule.o: debug.h
defmodule.o: debugcst.h
defmodule.o: def.h
defmodule.o: f_info.h
defmodule.o: idf.h
defmodule.o: input.h
defmodule.o: inputtype.h
defmodule.o: main.h
defmodule.o: misc.h
defmodule.o: node.h
defmodule.o: scope.h
defmodule.o: type.h
typequiv.o: LLlex.h
typequiv.o: debug.h
typequiv.o: debugcst.h
typequiv.o: def.h
typequiv.o: node.h
typequiv.o: type.h
typequiv.o: warning.h
node.o: LLlex.h
node.o: debug.h
node.o: debugcst.h
node.o: def.h
node.o: node.h
node.o: type.h
cstoper.o: LLlex.h
cstoper.o: Lpars.h
cstoper.o: debug.h
cstoper.o: debugcst.h
cstoper.o: idf.h
cstoper.o: node.h
cstoper.o: standards.h
cstoper.o: target_sizes.h
cstoper.o: type.h
cstoper.o: warning.h
chk_expr.o: LLlex.h
chk_expr.o: Lpars.h
chk_expr.o: chk_expr.h
chk_expr.o: const.h
chk_expr.o: debug.h
chk_expr.o: debugcst.h
chk_expr.o: def.h
chk_expr.o: idf.h
chk_expr.o: misc.h
chk_expr.o: node.h
chk_expr.o: scope.h
chk_expr.o: standards.h
chk_expr.o: type.h
chk_expr.o: warning.h
options.o: idfsize.h
options.o: main.h
options.o: ndir.h
options.o: type.h
options.o: warning.h
walk.o: LLlex.h
walk.o: Lpars.h
walk.o: chk_expr.h
walk.o: debug.h
walk.o: debugcst.h
walk.o: def.h
walk.o: desig.h
walk.o: f_info.h
walk.o: idf.h
walk.o: main.h
walk.o: node.h
walk.o: scope.h
walk.o: type.h
walk.o: walk.h
walk.o: warning.h
casestat.o: LLlex.h
casestat.o: Lpars.h
casestat.o: debug.h
casestat.o: debugcst.h
casestat.o: density.h
casestat.o: desig.h
casestat.o: node.h
casestat.o: type.h
casestat.o: walk.h
desig.o: LLlex.h
desig.o: debug.h
desig.o: debugcst.h
desig.o: def.h
desig.o: desig.h
desig.o: node.h
desig.o: scope.h
desig.o: type.h
code.o: LLlex.h
code.o: Lpars.h
code.o: debug.h
code.o: debugcst.h
code.o: def.h
code.o: desig.h
code.o: node.h
code.o: scope.h
code.o: standards.h
code.o: type.h
code.o: walk.h
tmpvar.o: debug.h
tmpvar.o: debugcst.h
tmpvar.o: def.h
tmpvar.o: main.h
tmpvar.o: scope.h
tmpvar.o: type.h
lookup.o: LLlex.h
lookup.o: debug.h
lookup.o: debugcst.h
lookup.o: def.h
lookup.o: idf.h
lookup.o: misc.h
lookup.o: node.h
lookup.o: scope.h
lookup.o: type.h
next.o: debug.h
next.o: debugcst.h
tokenfile.o: Lpars.h
program.o: LLlex.h
program.o: Lpars.h
program.o: debug.h
program.o: debugcst.h
program.o: def.h
program.o: f_info.h
program.o: idf.h
program.o: main.h
program.o: node.h
program.o: scope.h
program.o: type.h
program.o: warning.h
declar.o: LLlex.h
declar.o: Lpars.h
declar.o: chk_expr.h
declar.o: debug.h
declar.o: debugcst.h
declar.o: def.h
declar.o: idf.h
declar.o: main.h
declar.o: misc.h
declar.o: node.h
declar.o: scope.h
declar.o: type.h
declar.o: warning.h
expression.o: LLlex.h
expression.o: Lpars.h
expression.o: chk_expr.h
expression.o: const.h
expression.o: debug.h
expression.o: debugcst.h
expression.o: def.h
expression.o: idf.h
expression.o: node.h
expression.o: type.h
expression.o: warning.h
statement.o: LLlex.h
statement.o: Lpars.h
statement.o: def.h
statement.o: idf.h
statement.o: node.h
statement.o: scope.h
statement.o: type.h
Lpars.o: Lpars.h
