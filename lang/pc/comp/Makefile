# make iso-pascal "compiler"
EMHOME =	../../..
MDIR =		$(EMHOME)/modules
MHDIR =		$(MDIR)/h
PKGDIR =	$(MDIR)/pkg
LIBDIR =	$(MDIR)/lib
LLGEN =		$(EMHOME)/bin/LLgen
MKDEP =		$(EMHOME)/bin/mkdep
PRID =		$(EMHOME)/bin/prid
CID =		$(EMHOME)/bin/cid
OPR =		opr
CURRDIR =
CC =		fcc
CC =		cc
LINT =		lint
MACH =		sun3
BACK =		back

MALLOC =	$(LIBDIR)/malloc.o
EMELIB =	$(LIBDIR)/libem_mes.a \
		$(LIBDIR)/libeme.a \
		$(EMHOME)/lib/em_data.a
EMKLIB =	$(LIBDIR)/libem_mes.a $(LIBDIR)/libemk.a
MODLIB =	$(LIBDIR)/libinput.a \
		$(LIBDIR)/libassert.a \
		$(LIBDIR)/liballoc.a \
		$(MALLOC) \
		$(LIBDIR)/libprint.a \
		$(LIBDIR)/libstring.a \
		$(LIBDIR)/libsystem.a
EMCELIB =	$(LIBDIR)/libem_mesCE.a \
		$(EMHOME)/lib/$(MACH)/ce.a \
		$(EMHOME)/lib/$(MACH)/$(BACK).a \
		$(LIBDIR)/libflt.a \
		$(LIBDIR)/libobject.a \
		$(EMHOME)/lib/em_data.a
EMOCELIB =	$(LIBDIR)/libem_mesO.a \
		$(LIBDIR)/libCEopt.a \
		$(EMHOME)/lib/$(MACH)/ce.a \
		$(EMHOME)/lib/$(MACH)/$(BACK).a \
		$(LIBDIR)/libflt.a \
		$(LIBDIR)/libobject.a \
		$(EMHOME)/lib/em_data.a

LIBS =		$(EMKLIB) $(MODLIB)
CELIBS =	$(EMCELIB) $(MODLIB)
OLIBS =		$(EMOCELIB) $(MODLIB)

GFILES =	tokenfile.g declar.g expression.g program.g statement.g
LLGENOPTIONS =
PROFILE =
COPTIONS =
INCLUDES =	-I$(MHDIR) -I$(EMHOME)/h -I$(PKGDIR)
OPTIM=		-O
CFLAGS =	$(PROFILE) $(INCLUDES) $(OPTIM) $(COPTIONS) -DSTATIC=
LINTFLAGS =	-DSTATIC= -DNORCSID
LDFLAGS =	$(PROFILE)

LSRC =	declar.c expression.c program.c statement.c tokenfile.c
LOBJ =	declar.o expression.o program.o statement.o tokenfile.o
CSRC =	LLlex.c LLmessage.c body.c chk_expr.c code.c\
	cstoper.c def.c desig.c enter.c error.c idf.c input.c label.c\
	lookup.c main.c misc.c node.c options.c readwrite.c\
	scope.c symbol2str.c tokenname.c type.c typequiv.c progs.c stab.c
COBJ =	LLlex.o LLmessage.o body.o casestat.o char.o chk_expr.o code.o\
	cstoper.o def.o desig.o enter.o error.o idf.o input.o label.o\
	lookup.o main.o misc.o next.o node.o options.o readwrite.o scope.o\
	symbol2str.o tmpvar.o tokenname.o type.o typequiv.o progs.o stab.o

# Extra object for generating peephole-optimizer-code-expander version.
# The Sun-3 version is faster when the text-size exceeds 180K! ARRGH
EXTRA_O =

OBJ =	Lpars.o $(COBJ) $(LOBJ) $(EXTRA_O)

# Keep the next entries up to date!
GENCFILES=	$(LSRC) Lpars.c symbol2str.c casestat.c tmpvar.c char.c next.c
SRC =	$(CSRC) $(GENCFILES)
GENGFILES=	tokenfile.g
GENHFILES=	Lpars.h debugcst.h density.h errout.h idfsize.h inputtype.h\
	numsize.h strsize.h def.h type.h desig.h scope.h node.h\
	target_sizes.h nocross.h dbsymtab.h
HFILES=		LLlex.h chk_expr.h class.h const.h debug.h def.h desig.h\
	f_info.h idf.h input.h main.h misc.h node.h required.h scope.h\
	tokenname.h type.h $(GENHFILES)
#
GENFILES = $(GENGFILES) $(GENCFILES) $(GENHFILES)
NEXTFILES = def.H desig.H node.H scope.H type.H casestat.C tmpvar.C

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
	cp $(CURRDIR)main $(EMHOME)/lib/em_pc

cmp:	all
	-cmp $(CURRDIR)main $(EMHOME)/lib/em_pc

opr:
	make pr | $(OPR)

pr:
	@pr Makefile Resolve Parameters $(GFILES) *.H $(HFILES) *.C $(CSRC)

clean:
	rm -f $(OBJ) $(CURRDIR)main $(GENFILES) hfiles Cfiles LLfiles clashes \
		LL.output
	(cd .. ; rm -rf Xsrc)

lint:	Cfiles
	sh -c 'if $(CC) nmclash.c > /dev/null 2>&1 ; then make "EMHOME="$(EMHOME) Xlint ; else EMHOME=$(EMHOME); export EMHOME; sh Resolve Xlint ; fi'
	@rm -f nmclash.o a.out

longnames:	$(SRC) $(HFILES)
	sh -c 'if test -f longnames ; then $(PRID) -l7 longnames $? > Xlongnames ; mv Xlongnames longnames ; else $(PRID) -l7 $? > longnames ; fi'

# entry points not to be used directly

Cfiles: hfiles LLfiles $(GENCFILES) $(GENHFILES) Makefile
	echo $(SRC) $(HFILES) > Cfiles

LLfiles:	$(GFILES)
	$(LLGEN) $(LLGENOPTIONS) $(GFILES)
	@touch LLfiles

hfiles: Parameters make.hfiles
	make.hfiles Parameters
	touch hfiles

tokenfile.g:	tokenname.c make.tokfile
	make.tokfile < tokenname.c > tokenfile.g

symbol2str.c:	tokenname.c make.tokcase
	make.tokcase < tokenname.c > symbol2str.c

.SUFFIXES:	.H .h
.H.h:
		./make.allocd < $*.H > $*.h

.SUFFIXES:	.C .c
.C.c:
		./make.allocd < $*.C > $*.c

def.h:		make.allocd
type.h:		make.allocd
node.h:		make.allocd
scope.h:	make.allocd
desig.h:	make.allocd
casestat.c:	make.allocd
tmpvar.c:	make.allocd

next.c:		$(NEXTFILES) ./make.next
		./make.next $(NEXTFILES) > next.c

char.c: char.tab
	$(EMHOME)/bin/tabgen -fchar.tab > char.c

depend: Cfiles
	sed '/^#AUTOAUTO/,$$d' Makefile > Makefile.new
	echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
	$(MKDEP) $(SRC) |\
		sed 's/\.c:/\.o:/' >> Makefile.new
	mv Makefile Makefile.old
	mv Makefile.new Makefile

print:	$(CSRC) $(GFILES) $(HFILES)	# print recently changed files
	pr -t $? | $(OPR)
	@touch print

xref:
	ctags -x $(CSRC) $(HFILES) | sed "s/).*/)/">Xref

#INCLINCLINCLINCL

Xlint:
	$(LINT) $(INCLUDES) $(LINTFLAGS) $(SRC) \
		$(LIBDIR)/llib-lem_mes.ln \
		$(LIBDIR)/llib-lemk.ln \
		$(LIBDIR)/llib-linput.ln \
		$(LIBDIR)/llib-lassert.ln \
		$(LIBDIR)/llib-lalloc.ln \
		$(LIBDIR)/llib-lprint.ln \
		$(LIBDIR)/llib-lstring.ln \
		$(LIBDIR)/llib-lsystem.ln

$(CURRDIR)main: $(OBJ) $(CURRDIR)Makefile
	-mv $(CURRDIR)main $(CURRDIR)main.old
	$(CC) $(LDFLAGS) $(OBJ) $(LIBS) -o $(CURRDIR)main
	-size $(CURRDIR)main.old
	size $(CURRDIR)main

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
LLlex.o: const.h
LLlex.o: dbsymtab.h
LLlex.o: debug.h
LLlex.o: debugcst.h
LLlex.o: f_info.h
LLlex.o: idf.h
LLlex.o: idfsize.h
LLlex.o: input.h
LLlex.o: inputtype.h
LLlex.o: main.h
LLlex.o: nocross.h
LLlex.o: numsize.h
LLlex.o: strsize.h
LLlex.o: target_sizes.h
LLlex.o: type.h
LLmessage.o: LLlex.h
LLmessage.o: Lpars.h
LLmessage.o: dbsymtab.h
LLmessage.o: idf.h
LLmessage.o: nocross.h
LLmessage.o: target_sizes.h
LLmessage.o: type.h
body.o: LLlex.h
body.o: chk_expr.h
body.o: dbsymtab.h
body.o: debug.h
body.o: debugcst.h
body.o: def.h
body.o: desig.h
body.o: idf.h
body.o: main.h
body.o: misc.h
body.o: nocross.h
body.o: node.h
body.o: scope.h
body.o: target_sizes.h
body.o: type.h
chk_expr.o: LLlex.h
chk_expr.o: Lpars.h
chk_expr.o: chk_expr.h
chk_expr.o: const.h
chk_expr.o: dbsymtab.h
chk_expr.o: debug.h
chk_expr.o: debugcst.h
chk_expr.o: def.h
chk_expr.o: idf.h
chk_expr.o: main.h
chk_expr.o: misc.h
chk_expr.o: nocross.h
chk_expr.o: node.h
chk_expr.o: required.h
chk_expr.o: scope.h
chk_expr.o: target_sizes.h
chk_expr.o: type.h
code.o: LLlex.h
code.o: Lpars.h
code.o: dbsymtab.h
code.o: debug.h
code.o: debugcst.h
code.o: def.h
code.o: desig.h
code.o: f_info.h
code.o: idf.h
code.o: main.h
code.o: misc.h
code.o: nocross.h
code.o: node.h
code.o: required.h
code.o: scope.h
code.o: target_sizes.h
code.o: type.h
cstoper.o: LLlex.h
cstoper.o: Lpars.h
cstoper.o: const.h
cstoper.o: dbsymtab.h
cstoper.o: debug.h
cstoper.o: debugcst.h
cstoper.o: nocross.h
cstoper.o: node.h
cstoper.o: required.h
cstoper.o: target_sizes.h
cstoper.o: type.h
def.o: LLlex.h
def.o: dbsymtab.h
def.o: debug.h
def.o: debugcst.h
def.o: def.h
def.o: idf.h
def.o: main.h
def.o: misc.h
def.o: nocross.h
def.o: node.h
def.o: scope.h
def.o: target_sizes.h
def.o: type.h
desig.o: LLlex.h
desig.o: dbsymtab.h
desig.o: debug.h
desig.o: debugcst.h
desig.o: def.h
desig.o: desig.h
desig.o: idf.h
desig.o: main.h
desig.o: nocross.h
desig.o: node.h
desig.o: scope.h
desig.o: target_sizes.h
desig.o: type.h
enter.o: LLlex.h
enter.o: dbsymtab.h
enter.o: def.h
enter.o: idf.h
enter.o: main.h
enter.o: nocross.h
enter.o: node.h
enter.o: scope.h
enter.o: target_sizes.h
enter.o: type.h
error.o: LLlex.h
error.o: debug.h
error.o: debugcst.h
error.o: errout.h
error.o: f_info.h
error.o: input.h
error.o: inputtype.h
error.o: main.h
error.o: node.h
idf.o: idf.h
input.o: f_info.h
input.o: idf.h
input.o: input.h
input.o: inputtype.h
label.o: LLlex.h
label.o: dbsymtab.h
label.o: def.h
label.o: idf.h
label.o: main.h
label.o: nocross.h
label.o: node.h
label.o: scope.h
label.o: target_sizes.h
label.o: type.h
lookup.o: LLlex.h
lookup.o: dbsymtab.h
lookup.o: def.h
lookup.o: idf.h
lookup.o: misc.h
lookup.o: nocross.h
lookup.o: node.h
lookup.o: scope.h
lookup.o: target_sizes.h
lookup.o: type.h
main.o: LLlex.h
main.o: Lpars.h
main.o: class.h
main.o: const.h
main.o: dbsymtab.h
main.o: debug.h
main.o: debugcst.h
main.o: def.h
main.o: f_info.h
main.o: idf.h
main.o: input.h
main.o: inputtype.h
main.o: main.h
main.o: nocross.h
main.o: node.h
main.o: required.h
main.o: scope.h
main.o: target_sizes.h
main.o: tokenname.h
main.o: type.h
misc.o: LLlex.h
misc.o: f_info.h
misc.o: idf.h
misc.o: main.h
misc.o: misc.h
misc.o: node.h
node.o: LLlex.h
node.o: dbsymtab.h
node.o: debug.h
node.o: debugcst.h
node.o: nocross.h
node.o: node.h
node.o: target_sizes.h
node.o: type.h
options.o: class.h
options.o: const.h
options.o: dbsymtab.h
options.o: idfsize.h
options.o: main.h
options.o: nocross.h
options.o: target_sizes.h
options.o: type.h
readwrite.o: LLlex.h
readwrite.o: dbsymtab.h
readwrite.o: debug.h
readwrite.o: debugcst.h
readwrite.o: def.h
readwrite.o: idf.h
readwrite.o: main.h
readwrite.o: misc.h
readwrite.o: nocross.h
readwrite.o: node.h
readwrite.o: scope.h
readwrite.o: target_sizes.h
readwrite.o: type.h
scope.o: LLlex.h
scope.o: dbsymtab.h
scope.o: debug.h
scope.o: debugcst.h
scope.o: def.h
scope.o: idf.h
scope.o: misc.h
scope.o: nocross.h
scope.o: node.h
scope.o: scope.h
scope.o: target_sizes.h
scope.o: type.h
symbol2str.o: Lpars.h
tokenname.o: Lpars.h
tokenname.o: idf.h
tokenname.o: tokenname.h
type.o: LLlex.h
type.o: const.h
type.o: dbsymtab.h
type.o: debug.h
type.o: debugcst.h
type.o: def.h
type.o: idf.h
type.o: main.h
type.o: nocross.h
type.o: node.h
type.o: scope.h
type.o: target_sizes.h
type.o: type.h
typequiv.o: LLlex.h
typequiv.o: dbsymtab.h
typequiv.o: debug.h
typequiv.o: debugcst.h
typequiv.o: def.h
typequiv.o: nocross.h
typequiv.o: node.h
typequiv.o: target_sizes.h
typequiv.o: type.h
progs.o: LLlex.h
progs.o: dbsymtab.h
progs.o: debug.h
progs.o: debugcst.h
progs.o: def.h
progs.o: main.h
progs.o: nocross.h
progs.o: scope.h
progs.o: target_sizes.h
progs.o: type.h
stab.o: LLlex.h
stab.o: const.h
stab.o: dbsymtab.h
stab.o: def.h
stab.o: idf.h
stab.o: main.h
stab.o: nocross.h
stab.o: node.h
stab.o: scope.h
stab.o: target_sizes.h
stab.o: type.h
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
declar.o: scope.h
declar.o: target_sizes.h
declar.o: type.h
expression.o: LLlex.h
expression.o: Lpars.h
expression.o: chk_expr.h
expression.o: dbsymtab.h
expression.o: debug.h
expression.o: debugcst.h
expression.o: def.h
expression.o: idf.h
expression.o: main.h
expression.o: misc.h
expression.o: nocross.h
expression.o: node.h
expression.o: scope.h
expression.o: target_sizes.h
expression.o: type.h
program.o: LLlex.h
program.o: Lpars.h
program.o: dbsymtab.h
program.o: def.h
program.o: f_info.h
program.o: idf.h
program.o: main.h
program.o: node.h
program.o: scope.h
statement.o: LLlex.h
statement.o: Lpars.h
statement.o: chk_expr.h
statement.o: dbsymtab.h
statement.o: def.h
statement.o: desig.h
statement.o: f_info.h
statement.o: idf.h
statement.o: main.h
statement.o: misc.h
statement.o: nocross.h
statement.o: node.h
statement.o: scope.h
statement.o: target_sizes.h
statement.o: type.h
tokenfile.o: Lpars.h
Lpars.o: Lpars.h
symbol2str.o: Lpars.h
casestat.o: LLlex.h
casestat.o: Lpars.h
casestat.o: chk_expr.h
casestat.o: dbsymtab.h
casestat.o: debug.h
casestat.o: debugcst.h
casestat.o: density.h
casestat.o: main.h
casestat.o: nocross.h
casestat.o: node.h
casestat.o: target_sizes.h
casestat.o: type.h
tmpvar.o: dbsymtab.h
tmpvar.o: debug.h
tmpvar.o: debugcst.h
tmpvar.o: def.h
tmpvar.o: main.h
tmpvar.o: nocross.h
tmpvar.o: scope.h
tmpvar.o: target_sizes.h
tmpvar.o: type.h
char.o: class.h
next.o: debug.h
next.o: debugcst.h
