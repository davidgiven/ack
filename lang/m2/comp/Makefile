# make modula-2 "compiler"
EMHOME =	../../..
MDIR =		$(EMHOME)/modules
MHDIR =		$(MDIR)/h
PKGDIR =	$(MDIR)/pkg
LIBDIR =	$(MDIR)/lib
LLGEN =		$(EMHOME)/bin/LLgen
MKDEP =		$(EMHOME)/bin/mkdep
PRID =		$(EMHOME)/bin/prid
CID =		$(EMHOME)/bin/cid
CURRDIR =
LINT =		lint
MACH =		sun3
BACK =		back

MALLOC =	$(LIBDIR)/malloc.o
EMELIB =	$(LIBDIR)/libem_mes.a $(LIBDIR)/libeme.a
EMKLIB =	$(LIBDIR)/libem_mes.a $(LIBDIR)/libemk.a
MODLIB =	$(LIBDIR)/libinput.a \
		$(LIBDIR)/libassert.a \
		$(LIBDIR)/liballoc.a \
		$(MALLOC) \
		$(LIBDIR)/libflt.a \
		$(LIBDIR)/libprint.a \
		$(LIBDIR)/libstring.a \
		$(LIBDIR)/libsystem.a
EMCELIB =	$(LIBDIR)/libem_mesCE.a \
		$(EMHOME)/lib/$(MACH)/ce.a \
		$(EMHOME)/lib/$(MACH)/$(BACK).a \
		$(LIBDIR)/libobject.a \
		$(EMHOME)/lib/em_data.a
EMOCELIB =	$(LIBDIR)/libem_mesO.a \
		$(LIBDIR)/libCEopt.a \
		$(EMHOME)/lib/$(MACH)/ce.a \
		$(EMHOME)/lib/$(MACH)/$(BACK).a \
		$(LIBDIR)/libobject.a \
		$(EMHOME)/lib/em_data.a

LIBS =		$(EMKLIB) $(MODLIB)
CELIBS =	$(EMCELIB) $(MODLIB)
OLIBS =		$(EMOCELIB) $(MODLIB)

GF =		program.g declar.g expression.g statement.g
GENGFILES=	tokenfile.g
GFILES =	$(GENGFILES) $(GF)
LLGENOPTIONS = 	-v
PROFILE =
COPTIONS =
INCLUDES =	-I$(MHDIR) -I$(EMHOME)/h -I$(PKGDIR)
OPTIM =		-O
CFLAGS =	$(PROFILE) $(INCLUDES) $(COPTIONS) $(OPTIM)
LINTFLAGS =
LDFLAGS =	$(PROFILE)

LSRC =	tokenfile.c program.c declar.c expression.c statement.c
LOBJ =	tokenfile.o program.o declar.o expression.o statement.o
CSRC =	LLlex.c LLmessage.c error.c main.c \
	tokenname.c idf.c input.c type.c def.c \
	misc.c enter.c defmodule.c typequiv.c node.c \
	cstoper.c chk_expr.c options.c walk.c desig.c \
	code.c lookup.c Version.c stab.c
COBJ =	LLlex.o LLmessage.o char.o error.o main.o \
	symbol2str.o tokenname.o idf.o input.o type.o def.o \
	scope.o misc.o enter.o defmodule.o typequiv.o node.o \
	cstoper.o chk_expr.o options.o walk.o casestat.o desig.o \
	code.o tmpvar.o lookup.o Version.o stab.o next.o
GENC=	$(LSRC) symbol2str.c char.c Lpars.c casestat.c tmpvar.c scope.c next.c
SRC =	$(CSRC) $(GENC)

# Extra object for generating peephole-optimizer-code-expander version.
# The Sun-3 version is faster when the text-size exceeds 180K! ARRGH
EXTRA_O =

OBJ =	$(COBJ) $(LOBJ) Lpars.o $(EXTRA_O)

GENH =	errout.h \
	idfsize.h numsize.h strsize.h target_sizes.h bigresult.h \
	inputtype.h density.h squeeze.h nocross.h nostrict.h \
	def.h debugcst.h type.h Lpars.h node.h desig.h strict3rd.h real.h \
	use_insert.h dbsymtab.h uns_arith.h
HFILES =LLlex.h \
	chk_expr.h class.h debug.h f_info.h idf.h \
	input.h main.h misc.h scope.h standards.h tokenname.h \
	walk.h warning.h SYSTEM.h $(GENH)
#
GENFILES =	$(GENGFILES) $(GENC) $(GENH)
NEXTFILES =	def.H type.H node.H desig.H real.H scope.C tmpvar.C casestat.C

#EXCLEXCLEXCLEXCL

all:	Cfiles
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make "EMHOME="$(EMHOME) $(CURRDIR)main ; else EMHOME=$(EMHOME); export EMHOME; sh Resolve main ; fi'
	@rm -f nmclash.o a.out

Omain:	Cfiles
	rm -f *.o
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make "MACH="$(MACH) "EMHOME="$(EMHOME) "COPTIONS="-DPEEPHOLE "EXTRA_O="$(EXTRA_O) "BACK="$(BACK) $(CURRDIR)omain ; else MACH=$(MACH); export MACH; EMHOME=$(EMHOME); export EMHOME; ./Resolve omain ; fi'
	@rm -f nmclash.o a.out

CEmain: Cfiles
	rm -f *.o
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make "MACH="$(MACH) "EMHOME="$(EMHOME) "COPTIONS="-DCODE_EXPANDER "BACK="$(BACK) $(CURRDIR)cemain ; else MACH=$(MACH); export MACH; EMHOME=$(EMHOME); export EMHOME; ./Resolve cemain ; fi'
	@rm -f nmclash.o a.out

install:	all
	cp $(CURRDIR)main $(EMHOME)/lib/em_m2
	rm -f $(EMHOME)/man/em_m2.6 $(EMHOME)/man/modula-2.1
	cp $(CURRDIR)em_m2.6 $(CURRDIR)modula-2.1 $(EMHOME)/man

cmp:	all
	-cmp $(CURRDIR)main $(EMHOME)/lib/em_m2
	-cmp $(CURRDIR)em_m2.6 $(EMHOME)/man/em_m2.6
	-cmp $(CURRDIR)modula-2.1 $(EMHOME)/man/modula-2.1

opr:
	make pr | opr

pr:
	@pr Makefile Resolve Parameters $(GF) *.H $(HFILES) *.C $(CSRC)

clean:
	rm -f $(OBJ) $(GENFILES) LLfiles hfiles Cfiles clashes \
		$(CURRDIR)main LL.output
	(cd .. ; rm -rf Xsrc)

lint:	Cfiles
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make "EMHOME="$(EMHOME) Xlint ; else EMHOME=$(EMHOME); export EMHOME; sh Resolve Xlint ; fi'
	@rm -f nmclash.o a.out

longnames:	$(SRC) $(HFILES)
	sh -c 'if test -f longnames ; then $(PRID) -l7 longnames $? > Xlongnames ; mv Xlongnames longnames ; else $(PRID) -l7 $? > longnames ; fi'

# entry points not to be used directly

Cfiles: hfiles LLfiles $(GENC) $(GENH) Makefile
	echo $(SRC) $(HFILES) > Cfiles

LLfiles:	$(GFILES)
	$(LLGEN) $(LLGENOPTIONS) $(GFILES)
	@touch LLfiles

hfiles: Parameters make.hfiles
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
real.h:		make.allocd
node.h:		make.allocd
desig.h:	make.allocd
scope.c:	make.allocd
tmpvar.c:	make.allocd
casestat.c:	make.allocd

next.c:		$(NEXTFILES) ./make.next
		./make.next $(NEXTFILES) > next.c

char.c: char.tab
	$(EMHOME)/bin/tabgen -fchar.tab >char.c

depend: Cfiles
	sed '/^#AUTOAUTO/,$$d' Makefile > Makefile.new
	echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
	$(MKDEP) $(SRC) |\
		sed 's/\.c:/\.o:/' >> Makefile.new
	mv Makefile Makefile.old
	mv Makefile.new Makefile

#INCLINCLINCLINCL

Xlint:
	$(LINT) $(INCLUDES) $(LINTFLAGS) $(SRC) \
		$(LIBDIR)/llib-lem_mes.ln \
		$(LIBDIR)/llib-lemk.ln \
		$(LIBDIR)/llib-linput.ln \
		$(LIBDIR)/llib-lassert.ln \
		$(LIBDIR)/llib-lalloc.ln \
		$(LIBDIR)/llib-lflt.ln \
		$(LIBDIR)/llib-lprint.ln \
		$(LIBDIR)/llib-lstring.ln \
		$(LIBDIR)/llib-lsystem.ln

$(CURRDIR)main:	$(OBJ) $(CURRDIR)Makefile
	$(CC) $(LDFLAGS) $(OBJ) $(LIBS) -o $(CURRDIR)main
	-size $(CURRDIR)main

$(CURRDIR)omain:	$(OBJ) #$(CURRDIR)Makefile
	$(CC) $(LDFLAGS) $(OBJ) $(OLIBS) -o $(CURRDIR)omain
	-size $(CURRDIR)omain

$(CURRDIR)cemain:      $(OBJ) #$(CURRDIR)Makefile
	$(CC) $(LDFLAGS) $(OBJ) $(CELIBS) -o $(CURRDIR)cemain 
	-size $(CURRDIR)cemain


#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
LLlex.o: LLlex.h
LLlex.o: Lpars.h
LLlex.o: class.h
LLlex.o: dbsymtab.h
LLlex.o: debug.h
LLlex.o: debugcst.h
LLlex.o: def.h
LLlex.o: f_info.h
LLlex.o: idf.h
LLlex.o: idfsize.h
LLlex.o: input.h
LLlex.o: inputtype.h
LLlex.o: nocross.h
LLlex.o: numsize.h
LLlex.o: real.h
LLlex.o: strsize.h
LLlex.o: target_sizes.h
LLlex.o: type.h
LLlex.o: warning.h
LLmessage.o: LLlex.h
LLmessage.o: Lpars.h
LLmessage.o: idf.h
LLmessage.o: real.h
error.o: LLlex.h
error.o: debug.h
error.o: debugcst.h
error.o: errout.h
error.o: f_info.h
error.o: input.h
error.o: inputtype.h
error.o: main.h
error.o: node.h
error.o: nostrict.h
error.o: real.h
error.o: strict3rd.h
error.o: warning.h
main.o: LLlex.h
main.o: Lpars.h
main.o: SYSTEM.h
main.o: dbsymtab.h
main.o: debug.h
main.o: debugcst.h
main.o: def.h
main.o: f_info.h
main.o: idf.h
main.o: input.h
main.o: inputtype.h
main.o: nocross.h
main.o: node.h
main.o: real.h
main.o: scope.h
main.o: standards.h
main.o: strict3rd.h
main.o: target_sizes.h
main.o: tokenname.h
main.o: type.h
main.o: warning.h
tokenname.o: Lpars.h
tokenname.o: idf.h
tokenname.o: tokenname.h
idf.o: idf.h
input.o: f_info.h
input.o: input.h
input.o: inputtype.h
type.o: LLlex.h
type.o: chk_expr.h
type.o: dbsymtab.h
type.o: debug.h
type.o: debugcst.h
type.o: def.h
type.o: idf.h
type.o: nocross.h
type.o: node.h
type.o: nostrict.h
type.o: real.h
type.o: scope.h
type.o: squeeze.h
type.o: target_sizes.h
type.o: type.h
type.o: walk.h
type.o: warning.h
def.o: LLlex.h
def.o: Lpars.h
def.o: dbsymtab.h
def.o: debug.h
def.o: debugcst.h
def.o: def.h
def.o: idf.h
def.o: main.h
def.o: nocross.h
def.o: node.h
def.o: real.h
def.o: scope.h
def.o: target_sizes.h
def.o: type.h
def.o: warning.h
misc.o: LLlex.h
misc.o: f_info.h
misc.o: idf.h
misc.o: misc.h
misc.o: node.h
misc.o: real.h
enter.o: LLlex.h
enter.o: dbsymtab.h
enter.o: debug.h
enter.o: debugcst.h
enter.o: def.h
enter.o: f_info.h
enter.o: idf.h
enter.o: main.h
enter.o: misc.h
enter.o: nocross.h
enter.o: node.h
enter.o: real.h
enter.o: scope.h
enter.o: target_sizes.h
enter.o: type.h
defmodule.o: LLlex.h
defmodule.o: Lpars.h
defmodule.o: dbsymtab.h
defmodule.o: debug.h
defmodule.o: debugcst.h
defmodule.o: def.h
defmodule.o: f_info.h
defmodule.o: idf.h
defmodule.o: input.h
defmodule.o: inputtype.h
defmodule.o: main.h
defmodule.o: misc.h
defmodule.o: nocross.h
defmodule.o: node.h
defmodule.o: real.h
defmodule.o: scope.h
defmodule.o: target_sizes.h
defmodule.o: type.h
typequiv.o: LLlex.h
typequiv.o: dbsymtab.h
typequiv.o: debug.h
typequiv.o: debugcst.h
typequiv.o: def.h
typequiv.o: idf.h
typequiv.o: main.h
typequiv.o: nocross.h
typequiv.o: node.h
typequiv.o: real.h
typequiv.o: strict3rd.h
typequiv.o: target_sizes.h
typequiv.o: type.h
typequiv.o: warning.h
node.o: LLlex.h
node.o: dbsymtab.h
node.o: debug.h
node.o: debugcst.h
node.o: def.h
node.o: main.h
node.o: nocross.h
node.o: node.h
node.o: real.h
node.o: target_sizes.h
node.o: type.h
cstoper.o: LLlex.h
cstoper.o: Lpars.h
cstoper.o: dbsymtab.h
cstoper.o: debug.h
cstoper.o: debugcst.h
cstoper.o: idf.h
cstoper.o: nocross.h
cstoper.o: node.h
cstoper.o: real.h
cstoper.o: standards.h
cstoper.o: target_sizes.h
cstoper.o: type.h
cstoper.o: uns_arith.h
cstoper.o: warning.h
chk_expr.o: LLlex.h
chk_expr.o: Lpars.h
chk_expr.o: chk_expr.h
chk_expr.o: dbsymtab.h
chk_expr.o: debug.h
chk_expr.o: debugcst.h
chk_expr.o: def.h
chk_expr.o: idf.h
chk_expr.o: main.h
chk_expr.o: misc.h
chk_expr.o: nocross.h
chk_expr.o: node.h
chk_expr.o: nostrict.h
chk_expr.o: real.h
chk_expr.o: scope.h
chk_expr.o: standards.h
chk_expr.o: strict3rd.h
chk_expr.o: target_sizes.h
chk_expr.o: type.h
chk_expr.o: warning.h
options.o: class.h
options.o: dbsymtab.h
options.o: idfsize.h
options.o: main.h
options.o: nocross.h
options.o: nostrict.h
options.o: squeeze.h
options.o: strict3rd.h
options.o: target_sizes.h
options.o: type.h
options.o: warning.h
walk.o: LLlex.h
walk.o: Lpars.h
walk.o: bigresult.h
walk.o: chk_expr.h
walk.o: dbsymtab.h
walk.o: debug.h
walk.o: debugcst.h
walk.o: def.h
walk.o: desig.h
walk.o: f_info.h
walk.o: idf.h
walk.o: main.h
walk.o: misc.h
walk.o: nocross.h
walk.o: node.h
walk.o: real.h
walk.o: scope.h
walk.o: squeeze.h
walk.o: strict3rd.h
walk.o: target_sizes.h
walk.o: type.h
walk.o: use_insert.h
walk.o: walk.h
walk.o: warning.h
desig.o: LLlex.h
desig.o: dbsymtab.h
desig.o: debug.h
desig.o: debugcst.h
desig.o: def.h
desig.o: desig.h
desig.o: nocross.h
desig.o: node.h
desig.o: real.h
desig.o: scope.h
desig.o: squeeze.h
desig.o: target_sizes.h
desig.o: type.h
desig.o: walk.h
desig.o: warning.h
code.o: LLlex.h
code.o: Lpars.h
code.o: bigresult.h
code.o: dbsymtab.h
code.o: debug.h
code.o: debugcst.h
code.o: def.h
code.o: desig.h
code.o: nocross.h
code.o: node.h
code.o: real.h
code.o: scope.h
code.o: squeeze.h
code.o: standards.h
code.o: target_sizes.h
code.o: type.h
code.o: walk.h
lookup.o: LLlex.h
lookup.o: dbsymtab.h
lookup.o: debug.h
lookup.o: debugcst.h
lookup.o: def.h
lookup.o: idf.h
lookup.o: misc.h
lookup.o: nocross.h
lookup.o: node.h
lookup.o: real.h
lookup.o: scope.h
lookup.o: target_sizes.h
lookup.o: type.h
stab.o: LLlex.h
stab.o: dbsymtab.h
stab.o: def.h
stab.o: idf.h
stab.o: main.h
stab.o: nocross.h
stab.o: real.h
stab.o: scope.h
stab.o: target_sizes.h
stab.o: type.h
tokenfile.o: Lpars.h
program.o: LLlex.h
program.o: Lpars.h
program.o: dbsymtab.h
program.o: debug.h
program.o: debugcst.h
program.o: def.h
program.o: f_info.h
program.o: idf.h
program.o: main.h
program.o: misc.h
program.o: nocross.h
program.o: node.h
program.o: real.h
program.o: scope.h
program.o: strict3rd.h
program.o: target_sizes.h
program.o: type.h
program.o: warning.h
declar.o: LLlex.h
declar.o: Lpars.h
declar.o: chk_expr.h
declar.o: dbsymtab.h
declar.o: debug.h
declar.o: debugcst.h
declar.o: def.h
declar.o: idf.h
declar.o: main.h
declar.o: misc.h
declar.o: nocross.h
declar.o: node.h
declar.o: nostrict.h
declar.o: real.h
declar.o: scope.h
declar.o: strict3rd.h
declar.o: target_sizes.h
declar.o: type.h
declar.o: warning.h
expression.o: LLlex.h
expression.o: Lpars.h
expression.o: chk_expr.h
expression.o: dbsymtab.h
expression.o: debug.h
expression.o: debugcst.h
expression.o: def.h
expression.o: idf.h
expression.o: nocross.h
expression.o: node.h
expression.o: real.h
expression.o: target_sizes.h
expression.o: type.h
expression.o: warning.h
statement.o: LLlex.h
statement.o: Lpars.h
statement.o: dbsymtab.h
statement.o: def.h
statement.o: idf.h
statement.o: nocross.h
statement.o: node.h
statement.o: real.h
statement.o: scope.h
statement.o: target_sizes.h
statement.o: type.h
symbol2str.o: Lpars.h
char.o: class.h
Lpars.o: Lpars.h
casestat.o: LLlex.h
casestat.o: Lpars.h
casestat.o: chk_expr.h
casestat.o: dbsymtab.h
casestat.o: debug.h
casestat.o: debugcst.h
casestat.o: def.h
casestat.o: density.h
casestat.o: desig.h
casestat.o: nocross.h
casestat.o: node.h
casestat.o: real.h
casestat.o: squeeze.h
casestat.o: target_sizes.h
casestat.o: type.h
casestat.o: walk.h
tmpvar.o: LLlex.h
tmpvar.o: dbsymtab.h
tmpvar.o: debug.h
tmpvar.o: debugcst.h
tmpvar.o: def.h
tmpvar.o: main.h
tmpvar.o: nocross.h
tmpvar.o: real.h
tmpvar.o: scope.h
tmpvar.o: target_sizes.h
tmpvar.o: type.h
scope.o: LLlex.h
scope.o: dbsymtab.h
scope.o: debug.h
scope.o: debugcst.h
scope.o: def.h
scope.o: idf.h
scope.o: nocross.h
scope.o: node.h
scope.o: real.h
scope.o: scope.h
scope.o: target_sizes.h
scope.o: type.h
next.o: debug.h
next.o: debugcst.h
