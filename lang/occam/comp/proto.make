# $Header$

# make occam compiler

#PARAMS		do not remove this line!

UTIL_BIN = \
	$(UTIL_HOME)/bin
SRC_DIR = \
	$(SRC_HOME)/lang/occam/comp

TABGEN=	$(UTIL_BIN)/tabgen
LLGEN =	$(UTIL_BIN)/LLgen
LLGENOPTIONS = \
	-v

SRC_G =	$(SRC_DIR)/occam.g
GEN_G =
GFILES=	$(GEN_G) $(SRC_G)

SRC_C = \
	$(SRC_DIR)/builtin.c \
	$(SRC_DIR)/code.c \
	$(SRC_DIR)/em.c \
	$(SRC_DIR)/expr.c \
	$(SRC_DIR)/keytab.c \
	$(SRC_DIR)/report.c \
	$(SRC_DIR)/symtab.c
GEN_C =	occam.c lex.yy.c Lpars.c
CFILES=	$(SRC_C) $(GEN_C)

SRC_H = \
	$(SRC_DIR)/code.h \
	$(SRC_DIR)/em.h \
	$(SRC_DIR)/expr.h \
	$(SRC_DIR)/sizes.h \
	$(SRC_DIR)/symtab.h \
	$(SRC_DIR)/token.h
GEN_H = Lpars.h
HFILES=	$(GEN_H) $(SRC_H)

SRC_L =	$(SRC_DIR)/lex.l
GEN_L =
LFILES = $(SRC_L) $(GEN_L)

all:		make.main
		make -f make.main main

install:	all
		@-mkdir $(TARGET_HOME)
		@-mkdir $(TARGET_HOME)/lib.bin
		cp main $(TARGET_HOME)/lib.bin/em_occam

cmp:		all
		-cmp main $(TARGET_HOME)/lib.bin/em_occam

opr:
		make pr | opr

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/proto.main \
			$(SRC_L) $(SRC_G) $(SRC_H) $(SRC_C)

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
		rm -f $(GEN_C) $(GEN_G) $(GEN_H) $(GEN_L) LLfiles Cfiles LL.output
		rm -f resolved *.dep lists make.main make_macros

LLfiles:	$(GFILES)
		$(LLGEN) $(LLGENOPTIONS) $(GFILES)
		@touch LLfiles

lex.yy.c:	$(LFILES)
		flex -s $(LFILES)
