# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/grind
UTIL_BIN = $(UTIL_HOME)/bin
TABGEN=	$(UTIL_BIN)/tabgen
LLGEN =	$(UTIL_BIN)/LLgen
LLGENOPTIONS = -v

SRC_G1 =	$(SRC_DIR)/commands.g
GEN_G1 =	tokenfile.g
GFILES1=	$(GEN_G1) $(SRC_G1)

SRC_G2 =	$(SRC_DIR)/db_symtab.g
GEN_G2 =	
GFILES2 =	$(GEN_G2) $(SRC_G2)

SRC_C = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/list.c \
	$(SRC_DIR)/tree.c \
	$(SRC_DIR)/expr.c \
	$(SRC_DIR)/position.c \
	$(SRC_DIR)/tokenname.c \
	$(SRC_DIR)/idf.c \
	$(SRC_DIR)/run.c \
	$(SRC_DIR)/symbol.c \
	$(SRC_DIR)/print.c \
	$(SRC_DIR)/type.c \
	$(SRC_DIR)/rd.c \
	$(SRC_DIR)/do_comm.c \
	$(SRC_DIR)/modula-2.c \
	$(SRC_DIR)/pascal.c \
	$(SRC_DIR)/c.c
GEN_C = db_symtab.c commands.c Lpars.c DBSpars.c avl.c scope.c itemlist.c \
	langdep.c ops.c chtab.c symbol2str.c
CFILES=	$(SRC_C) $(GEN_C)

SRC_H = \
	$(SRC_DIR)/tokenname.h \
	$(SRC_DIR)/operator.h \
	$(SRC_DIR)/class.h \
	$(SRC_DIR)/position.h \
	$(SRC_DIR)/idf.h \
	$(SRC_DIR)/message.h \
	$(SRC_DIR)/avl.h \
	$(SRC_DIR)/scope.h \
	$(SRC_DIR)/langdep.h \
	$(SRC_DIR)/sizes.h \
	$(SRC_DIR)/token.h \
	$(SRC_DIR)/expr.h \
	$(SRC_DIR)/rd.h
GEN_H = file.h type.h symbol.h tree.h Lpars.h DBSpars.h ops.h
HFILES=	$(GEN_H) $(SRC_H)

NEXTFILES =	$(SRC_DIR)/file.hh \
		$(SRC_DIR)/type.hh \
		$(SRC_DIR)/symbol.hh \
		$(SRC_DIR)/tree.hh \
		$(SRC_DIR)/avl.cc \
		$(SRC_DIR)/scope.cc \
		$(SRC_DIR)/itemlist.cc \
		$(SRC_DIR)/langdep.cc

all:		make.main
		make -f make.main grind

install:	all
		cp grind $(TARGET_HOME)/bin/grind
		cp $(SRC_DIR)/grind.1 $(TARGET_HOME)/man/grind.1

cmp:		all
		-cmp grind $(TARGET_HOME)/bin/grind
		-cmp $(SRC_DIR)/grind.1 $(TARGET_HOME)/man/grind.1

opr:
		make pr | opr

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/proto.main \
			$(SRC_DIR)/char.ct operators.ot \
			$(SRC_G1) $(SRC_G2) \
			$(SRC_H) $(NEXTFILES) \
			$(SRC_C)

lint:		make.main
		make -f make.main lint

Cfiles:		hfiles LLfiles $(GEN_C) $(GEN_H) Makefile
		echo $(CFILES) | tr ' ' '\012' > Cfiles
		echo $(HFILES) | tr ' ' '\012' >> Cfiles

resolved:	Cfiles
		CC="$(CC)" UTIL_HOME="$(UTIL_HOME)" do_resolve `cat Cfiles` > Cfiles.new
		-if cmp -s Cfiles Cfiles.new ; then rm -f Cfiles.new ; else mv Cfiles.new Cfiles ; fi
		touch resolved

# there is no file called "dependencies"; we want dependencies checked 
# every time. This means that make.main is made every time. Oh well ...
# it does not take much time.
dependencies:	resolved
		do_deps `grep '.c$$' Cfiles`

make.main:	dependencies make_macros lists $(SRC_DIR)/proto.main
		sed -e '/^#DEPENDENCIES/,$$d' -e '/^#PARAMS/r make_macros' -e '/^#LISTS/r lists' $(SRC_DIR)/proto.main > make.main
		echo '#DEPENDENCIES' >> make.main
		cat *.dep >> make.main

make_macros:	Makefile
		echo 'SRC_DIR=$(SRC_DIR)' > make_macros
		echo 'UTIL_HOME=$(UTIL_HOME)' >> make_macros
		echo 'TARGET_HOME=$(TARGET_HOME)' >> make_macros
		echo 'CC=$(CC)' >> make_macros
		echo 'COPTIONS=$(COPTIONS)' >> make_macros
		echo 'LDOPTIONS=$(LDOPTIONS)' >> make_macros
		echo 'LINT=$(LINT)' >> make_macros
		echo 'LINTOPTIONS=$(LINTOPTIONS)' >> make_macros
		echo 'SUF=$(SUF)' >> make_macros
		echo 'LIBSUF=$(LIBSUF)' >> make_macros
		echo 'CC_AND_MKDEP=$(CC_AND_MKDEP)' >> make_macros

lists:		Cfiles
		echo "C_SRC = \\" > lists
		echo $(CFILES) >> lists
		echo "OBJ = \\" >> lists
		echo $(CFILES) | sed -e 's#[^ ]*/##g' -e 's/\.c/.$$(SUF)/g' >> lists

clean:
		-make -f make.main clean
		rm -f $(GEN_C) $(GEN_G) $(GEN_H) \
			hfiles LL1files LL2files Cfiles LL.output DBS.output
		rm -f resolved *.dep lists make.main make_macros

LLfiles:	LL1files LL2files

LL1files:	$(GFILES1)
		$(LLGEN) $(LLGENOPTIONS) $(GFILES1)
		@touch LL1files

LL2files:	$(GFILES2)
		$(LLGEN) $(LLGENOPTIONS) $(GFILES2)
		@touch LL2files

hfiles:		Parameters $(SRC_DIR)/make.hfiles
		$(SRC_DIR)/make.hfiles Parameters
		touch hfiles

tokenfile.g:	$(SRC_DIR)/tokenname.c $(SRC_DIR)/make.tokfile
		$(SRC_DIR)/make.tokfile <$(SRC_DIR)/tokenname.c >tokenfile.g

symbol2str.c:	$(SRC_DIR)/tokenname.c $(SRC_DIR)/make.tokcase
		$(SRC_DIR)/make.tokcase <$(SRC_DIR)/tokenname.c >symbol2str.c

file.h:		$(SRC_DIR)/file.hh $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/file.hh > file.h

symbol.h:	$(SRC_DIR)/symbol.hh $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/symbol.hh > symbol.h

tree.h:		$(SRC_DIR)/tree.hh $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/tree.hh > tree.h

type.h:		$(SRC_DIR)/type.hh $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/type.hh > type.h

avl.c:		$(SRC_DIR)/avl.cc $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/avl.cc > avl.c

itemlist.c:	$(SRC_DIR)/itemlist.cc $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/itemlist.cc > itemlist.c

langdep.c:	$(SRC_DIR)/langdep.cc $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/langdep.cc > langdep.c

scope.c:	$(SRC_DIR)/scope.cc $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/scope.cc > scope.c

next.c:		$(NEXTFILES) $(SRC_DIR)/make.next
		$(SRC_DIR)/make.next $(NEXTFILES) > next.c

chtab.c:	$(SRC_DIR)/char.ct
		$(TABGEN) -f$(SRC_DIR)/char.ct >chtab.c

ops.c ops.h:	$(SRC_DIR)/operators.ot $(SRC_DIR)/make.ops
		$(SRC_DIR)/make.ops $(SRC_DIR)/operators.ot
