# $Id$

# make Modula-2 compiler

#PARAMS		do not remove this line!

UTIL_BIN = \
	$(UTIL_HOME)/bin
SRC_DIR = \
	$(SRC_HOME)/lang/m2/comp

TABGEN=	$(UTIL_BIN)/tabgen
LLGEN =	$(UTIL_BIN)/LLgen
LLGENOPTIONS = \
	-n

SRC_G =	$(SRC_DIR)/program.g $(SRC_DIR)/declar.g \
	$(SRC_DIR)/expression.g $(SRC_DIR)/statement.g
GEN_G =	tokenfile.g
GFILES=	$(GEN_G) $(SRC_G)

SRC_C =	$(SRC_DIR)/LLlex.c $(SRC_DIR)/LLmessage.c $(SRC_DIR)/error.c \
	$(SRC_DIR)/main.c $(SRC_DIR)/tokenname.c $(SRC_DIR)/idf.c \
	$(SRC_DIR)/input.c $(SRC_DIR)/type.c $(SRC_DIR)/def.c \
	$(SRC_DIR)/misc.c $(SRC_DIR)/enter.c $(SRC_DIR)/defmodule.c \
	$(SRC_DIR)/typequiv.c $(SRC_DIR)/node.c $(SRC_DIR)/cstoper.c \
	$(SRC_DIR)/chk_expr.c $(SRC_DIR)/options.c $(SRC_DIR)/walk.c \
	$(SRC_DIR)/desig.c $(SRC_DIR)/code.c $(SRC_DIR)/lookup.c \
	$(SRC_DIR)/stab.c
GEN_C =	tokenfile.c program.c declar.c expression.c statement.c \
	symbol2str.c char.c Lpars.c Lncor.c casestat.c tmpvar.c scope.c next.c
CFILES=	$(SRC_C) $(GEN_C)

SRC_H =	$(SRC_DIR)/LLlex.h $(SRC_DIR)/chk_expr.h $(SRC_DIR)/class.h \
	$(SRC_DIR)/debug.h $(SRC_DIR)/desig.h $(SRC_DIR)/f_info.h \
	$(SRC_DIR)/idf.h $(SRC_DIR)/input.h $(SRC_DIR)/main.h \
	$(SRC_DIR)/misc.h $(SRC_DIR)/scope.h $(SRC_DIR)/standards.h \
	$(SRC_DIR)/tokenname.h $(SRC_DIR)/walk.h $(SRC_DIR)/warning.h \
	$(SRC_DIR)/SYSTEM.h
GEN_H =	errout.h idfsize.h numsize.h strsize.h target_sizes.h bigresult.h \
	inputtype.h density.h squeeze.h nocross.h nostrict.h def.h debugcst.h \
	type.h Lpars.h node.h strict3rd.h real.h use_insert.h dbsymtab.h \
	uns_arith.h def.h type.h node.h real.h
HFILES=	$(GEN_H) $(SRC_H)

NEXTFILES = \
	$(SRC_DIR)/def.H $(SRC_DIR)/type.H $(SRC_DIR)/node.H $(SRC_DIR)/real.H \
	$(SRC_DIR)/scope.C $(SRC_DIR)/tmpvar.C $(SRC_DIR)/casestat.C

all:		make.main
		make -f make.main main

install:	all
		cp main $(TARGET_HOME)/lib.bin/em_m2
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/em_m2.6 $(TARGET_HOME) ; \
			mk_manpage $(SRC_DIR)/modula-2.1 $(TARGET_HOME) ; \
		fi

cmp:		all
		-cmp main $(TARGET_HOME)/lib.bin/em_m2

opr:
		make pr | opr

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/proto.main Parameters \
			$(SRC_DIR)/char.tab $(SRC_G) $(SRC_H) $(NEXTFILES) $(SRC_C)

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
		rm_deps $(SRC_DIR)/proto.main | sed -e '/^.PARAMS/r make_macros' -e '/^.LISTS/r lists' > make.main
		cat *.dep >> make.main

make_macros:    Makefile
		echo 'SRC_DIR=$(SRC_DIR)' > make_macros
		echo 'UTIL_HOME=$(UTIL_HOME)' >> make_macros
		echo 'TARGET_HOME=$(TARGET_HOME)' >> make_macros
		echo 'CC=$(CC)' >> make_macros
		echo 'COPTIONS=$(COPTIONS)' >> make_macros
		echo 'LDOPTIONS=$(LDOPTIONS)' >> make_macros
		echo 'LINT=$(LINT)' >> make_macros
		echo 'LINTSUF=$(LINTSUF)' >> make_macros
		echo 'LINTPREF=$(LINTPREF)' >> make_macros
		echo 'LINTOPTIONS=$(LINTOPTIONS)' >> make_macros
		echo 'SUF=$(SUF)' >> make_macros
		echo 'LIBSUF=$(LIBSUF)' >> make_macros
		echo 'CC_AND_MKDEP=$(CC_AND_MKDEP)' >> make_macros

lists:		Cfiles
		echo "C_SRC = \\" > lists
		echo $(CFILES) >> lists
		echo "OBJ = \\" >> lists
		echo $(CFILES) | sed -e 's|[^ ]*/||g' -e 's/\.c/.$$(SUF)/g' >> lists

clean:
		-make -f make.main clean
		rm -f $(GEN_C) $(GEN_G) $(GEN_H) hfiles LLfiles Cfiles LL.output
		rm -f resolved *.dep lists make.main make_macros

LLfiles:	$(GFILES)
		$(LLGEN) $(LLGENOPTIONS) $(GFILES)
		@touch LLfiles
		@if [ -f Lncor.c ] ; then : ; else touch Lncor.c ; fi

hfiles:		Parameters $(SRC_DIR)/make.hfiles
		$(SRC_DIR)/make.hfiles Parameters
		touch hfiles

tokenfile.g:	$(SRC_DIR)/tokenname.c $(SRC_DIR)/make.tokfile
		$(SRC_DIR)/make.tokfile <$(SRC_DIR)/tokenname.c >tokenfile.g

symbol2str.c:	$(SRC_DIR)/tokenname.c $(SRC_DIR)/make.tokcase
		$(SRC_DIR)/make.tokcase <$(SRC_DIR)/tokenname.c >symbol2str.c

def.h:		$(SRC_DIR)/make.allocd $(SRC_DIR)/def.H
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/def.H > def.h

type.h:		$(SRC_DIR)/make.allocd $(SRC_DIR)/type.H
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/type.H > type.h

real.h:		$(SRC_DIR)/make.allocd $(SRC_DIR)/real.H
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/real.H > real.h

node.h:		$(SRC_DIR)/make.allocd $(SRC_DIR)/node.H
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/node.H > node.h

scope.c:	$(SRC_DIR)/make.allocd $(SRC_DIR)/scope.C
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/scope.C > scope.c

tmpvar.c:	$(SRC_DIR)/make.allocd $(SRC_DIR)/tmpvar.C
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/tmpvar.C > tmpvar.c

casestat.c:	$(SRC_DIR)/make.allocd $(SRC_DIR)/casestat.C
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/casestat.C > casestat.c

next.c:		$(NEXTFILES) $(SRC_DIR)/make.next
		$(SRC_DIR)/make.next $(NEXTFILES) > next.c

char.c:		$(SRC_DIR)/char.tab
		$(TABGEN) -f$(SRC_DIR)/char.tab >char.c
