# $Header$

# make basic compiler

#PARAMS		do not remove this line!

UTIL_BIN = \
	$(UTIL_HOME)/bin
SRC_DIR = \
	$(SRC_HOME)/lang/basic/src

TABGEN=	$(UTIL_BIN)/tabgen
LLGEN =	$(UTIL_BIN)/LLgen
LLGENOPTIONS = \
	-v

SRC_G =	$(SRC_DIR)/basic.g
GEN_G =
GFILES=	$(GEN_G) $(SRC_G)

SRC_C = \
	$(SRC_DIR)/bem.c \
	$(SRC_DIR)/symbols.c \
	$(SRC_DIR)/initialize.c \
	$(SRC_DIR)/compile.c \
	$(SRC_DIR)/parsepar.c \
	$(SRC_DIR)/gencode.c \
	$(SRC_DIR)/util.c \
	$(SRC_DIR)/graph.c \
	$(SRC_DIR)/eval.c \
	$(SRC_DIR)/func.c
GEN_C =	basic.c Lpars.c
CFILES=	$(SRC_C) $(GEN_C)

SRC_H = \
	$(SRC_DIR)/bem.h \
	$(SRC_DIR)/symbols.h \
	$(SRC_DIR)/graph.h \
	$(SRC_DIR)/llmess.c \
	$(SRC_DIR)/yylexp.c
GEN_H = Lpars.h tokentab.h
HFILES=	$(GEN_H) $(SRC_H)

all:		make.main
		make -f make.main main

install:	all
		@-mkdir $(TARGET_HOME)
		@-mkdir $(TARGET_HOME)/lib.bin
		cp main $(TARGET_HOME)/lib.bin/em_bem

cmp:		all
		-cmp main $(TARGET_HOME)/lib.bin/em_bem

opr:
		make pr | opr

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/proto.main \
			$(SRC_DIR)/maketokentab $(SRC_DIR)/basic.lex \
			$(SRC_G) $(SRC_H) $(SRC_C)

lint:		make.main
		make -f make.main lint

Cfiles:		LLfiles $(GEN_C) $(GEN_H) Makefile
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

make_macros:	Makefile
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
		rm -f $(GEN_C) $(GEN_G) $(GEN_H) $(GEN_L) LLfiles Cfiles LL.output
		rm -f resolved *.dep lists make.main make_macros

LLfiles:	$(GFILES)
		$(LLGEN) $(LLGENOPTIONS) $(GFILES)
		@touch LLfiles

tokentab.h:	Lpars.h $(SRC_DIR)/maketokentab
		$(SRC_DIR)/maketokentab
