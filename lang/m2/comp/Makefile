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
CFLAGS =	-DDEBUG -p $(INCLUDES)
LFLAGS =	-p
LOBJ =	tokenfile.o program.o declar.o expression.o statement.o
COBJ =	LLlex.o LLmessage.o char.o error.o main.o \
	symbol2str.o tokenname.o idf.o input.o type.o def.o \
	scope.o misc.o print.o enter.o
OBJ =	$(COBJ) $(LOBJ) Lpars.o
GENFILES=	tokenfile.c \
	program.c declar.c expression.c statement.c \
	tokenfile.g symbol2str.c char.c Lpars.c Lpars.h

all:
	make LLfiles
	make main

LLfiles:	$(LSRC)
	$(GEN) $(GENOPTIONS) $(LSRC)
	@touch LLfiles

main:	$(OBJ) Makefile
	$(CC) $(LFLAGS) $(OBJ) $(LIBDIR)/libcomp.a $(LIBDIR)/malloc.o /user1/erikb/em/lib/libstr.a /user1/erikb/em/lib/libsystem.a -o main
	size main

clean:
	rm -f $(OBJ) $(GENFILES) LLfiles 

tokenfile.g:	tokenname.c make.tokfile
	make.tokfile <tokenname.c >tokenfile.g

symbol2str.c:	tokenname.c make.tokcase
	make.tokcase <tokenname.c >symbol2str.c

misc.h:		misc.H make.allocd
def.h:		def.H make.allocd
type.h:		type.H make.allocd
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
LLlex.o: LLlex.h Lpars.h class.h f_info.h idf.h input.h
LLmessage.o: LLlex.h Lpars.h idf.h
char.o: class.h
error.o: LLlex.h f_info.h input.h
main.o: LLlex.h Lpars.h debug.h def.h f_info.h idf.h input.h main.h standards.h type.h
symbol2str.o: Lpars.h
tokenname.o: Lpars.h idf.h tokenname.h
idf.o: idf.h
input.o: f_info.h input.h
type.o: Lpars.h def.h def_sizes.h idf.h type.h
def.o: Lpars.h debug.h def.h idf.h main.h scope.h
scope.o: LLlex.h debug.h def.h idf.h scope.h type.h
misc.o: LLlex.h f_info.h idf.h misc.h
enter.o: def.h idf.h misc.h scope.h type.h
tokenfile.o: Lpars.h
program.o: LLlex.h Lpars.h def.h idf.h main.h misc.h scope.h type.h
declar.o: LLlex.h Lpars.h def.h idf.h misc.h scope.h type.h
expression.o: LLlex.h Lpars.h def.h idf.h scope.h
statement.o: Lpars.h
Lpars.o: Lpars.h
