#
EMHOME =	../../..
MHDIR =		$(EMHOME)/modules/h
PKGDIR =	$(EMHOME)/modules/pkg
LIBDIR =	$(EMHOME)/modules/lib
LLGEN =		$(EMHOME)/bin/LLgen
MKDEP =		$(EMHOME)/bin/mkdep

INCLUDES = -I$(MHDIR) -I$(PKGDIR) -I$(EMHOME)/h

GF =	program.g declar.g expression.g statement.g
GENGFILES=	tokenfile.g
GFILES =$(GENGFILES) $(GF)
LLGENOPTIONS =
PROFILE =
M2COMPILER = m2
DEFDIR = lib/m2
HOMEDIR =
CFLAGS = -O $(PROFILE) $(INCLUDES) -DM2COMPILER=\"$(M2COMPILER)\" -DDEF_DIR=\"$(DEFDIR)\" $(HOMEDIR)
LINTFLAGS = -DNORCSID
MALLOC = $(LIBDIR)/malloc.o
LDFLAGS = $(PROFILE)
LSRC =	tokenfile.c program.c declar.c expression.c statement.c
LOBJ =	tokenfile.o program.o declar.o expression.o statement.o
CSRC =	LLlex.c LLmessage.c error.c main.c lib.c \
	tokenname.c idf.c input.c misc.c options.c Version.c
COBJ =	LLlex.o LLmessage.o error.o main.o lib.o \
	tokenname.o idf.o input.o misc.o options.o char.o symbol2str.o Version.o
GENC=	$(LSRC) symbol2str.c char.c Lpars.c
SRC =	$(CSRC) $(GENC)
OBJ =	$(COBJ) $(LOBJ) Lpars.o
GENH =	Lpars.h
HSRC =	main.h LLlex.h class.h f_info.h idf.h input.h tokenname.h
HFILES =$(GENH) $(HSRC)
#
GENFILES = $(GENGFILES) $(GENC) $(GENH)

all:	Cfiles
	make "EMHOME="$(EMHOME) "DEFDIR="$(DEFDIR) "HOMEDIR="$(HOMEDIR) "M2COMPILER="$(M2COMPILER) m2mm

install:	all
	cp m2mm $(EMHOME)/bin
	cp m2mm.1 $(EMHOME)/man

cmp:	all
	-cmp m2mm $(EMHOME)/bin/m2mm
	-cmp m2mm.1 $(EMHOME)/man/m2mm.1

opr:
	make "EMHOME="$(EMHOME) pr | opr

pr:
	@pr Makefile $(GF) $(HFILES) $(CSRC)

clean:
	rm -f $(OBJ) $(GENFILES) LLfiles Cfiles LL.output m2mm

lint:	Cfiles
	lint $(INCLUDES) $(LINTFLAGS) $(SRC) \
		$(LIBDIR)/llib-linput.ln \
		$(LIBDIR)/llib-lalloc.ln \
		$(LIBDIR)/llib-lprint.ln \
		$(LIBDIR)/llib-lstring.ln \
		$(LIBDIR)/llib-lsystem.ln

# entry points not to be used directly
Cfiles:	LLfiles $(GENC) $(GENH) Makefile

LLfiles:	$(GFILES)
	$(LLGEN) $(LLGENOPTIONS) $(GFILES)
	@touch LLfiles

tokenfile.g:	tokenname.c make.tokfile
	make.tokfile <tokenname.c >tokenfile.g

symbol2str.c:	tokenname.c make.tokcase
	make.tokcase <tokenname.c >symbol2str.c

char.c: char.tab
	$(EMHOME)/bin/tabgen -fchar.tab >char.c

depend:	Cfiles
	sed '/^#AUTOAUTO/,$$d' Makefile > Makefile.new
	echo '#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO' >> Makefile.new
	$(MKDEP) $(SRC) |\
		sed 's/\.c:/\.o:/' >> Makefile.new
	mv Makefile Makefile.old
	mv Makefile.new Makefile

m2mm:	$(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LIBDIR)/libinput.a $(LIBDIR)/liballoc.a $(MALLOC) $(LIBDIR)/libprint.a $(LIBDIR)/libstring.a $(LIBDIR)/libsystem.a -o m2mm
	-size m2mm

#AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
LLlex.o: LLlex.h
LLlex.o: Lpars.h
LLlex.o: class.h
LLlex.o: f_info.h
LLlex.o: file_list.h
LLlex.o: idf.h
LLlex.o: idfsize.h
LLlex.o: input.h
LLlex.o: inputtype.h
LLmessage.o: LLlex.h
LLmessage.o: Lpars.h
LLmessage.o: file_list.h
LLmessage.o: idf.h
error.o: LLlex.h
error.o: f_info.h
error.o: input.h
error.o: inputtype.h
main.o: LLlex.h
main.o: Lpars.h
main.o: f_info.h
main.o: file_list.h
main.o: idf.h
main.o: input.h
main.o: inputtype.h
main.o: tokenname.h
tokenname.o: Lpars.h
tokenname.o: file_list.h
tokenname.o: idf.h
tokenname.o: tokenname.h
idf.o: file_list.h
idf.o: idf.h
input.o: f_info.h
input.o: input.h
input.o: inputtype.h
misc.o: LLlex.h
misc.o: f_info.h
misc.o: file_list.h
misc.o: idf.h
options.o: main.h
tokenfile.o: Lpars.h
program.o: LLlex.h
program.o: Lpars.h
program.o: f_info.h
program.o: file_list.h
program.o: idf.h
program.o: main.h
declar.o: Lpars.h
expression.o: Lpars.h
statement.o: LLlex.h
statement.o: Lpars.h
statement.o: file_list.h
statement.o: idf.h
symbol2str.o: Lpars.h
char.o: class.h
Lpars.o: Lpars.h
