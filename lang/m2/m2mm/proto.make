# $Header$

# make Modula-2 makefile generator

#PARAMS		do not remove this line!

UTIL_BIN = \
	$(UTIL_HOME)/bin
SRC_DIR = \
	$(SRC_HOME)/lang/m2/m2mm

TABGEN=	$(UTIL_BIN)/tabgen
LLGEN =	$(UTIL_BIN)/LLgen
LLGENOPTIONS = \
	-v

SRC_G =	$(SRC_DIR)/program.g $(SRC_DIR)/declar.g \
	$(SRC_DIR)/expression.g $(SRC_DIR)/statement.g
GEN_G =	tokenfile.g
GFILES=	$(GEN_G) $(SRC_G)

SRC_C =	$(SRC_DIR)/LLlex.c $(SRC_DIR)/LLmessage.c $(SRC_DIR)/error.c \
	$(SRC_DIR)/main.c $(SRC_DIR)/lib.c $(SRC_DIR)/tokenname.c \
	$(SRC_DIR)/idf.c $(SRC_DIR)/input.c $(SRC_DIR)/misc.c \
	$(SRC_DIR)/options.c $(SRC_DIR)/Version.c 
GEN_C =	tokenfile.c program.c declar.c expression.c statement.c \
	symbol2str.c char.c Lpars.c
CFILES=	$(SRC_C) $(GEN_C)

SRC_H =	$(SRC_DIR)/LLlex.h $(SRC_DIR)/class.h \
	$(SRC_DIR)/f_info.h \
	$(SRC_DIR)/idf.h $(SRC_DIR)/input.h $(SRC_DIR)/main.h \
	$(SRC_DIR)/tokenname.h
GEN_H =	Lpars.h
HFILES=	$(GEN_H) $(SRC_H)

all:		make.main
		make -f make.main main

install:	all
		@-mkdir $(TARGET_HOME)
		@-mkdir $(TARGET_HOME)/bin
		cp main $(TARGET_HOME)/bin/m2mm
		@-mkdir $(TARGET_HOME)/man
		cp $(SRC_DIR)/m2mm.1 $(TARGET_HOME)/man/m2mm.1

cmp:		all
		-cmp main $(TARGET_HOME)/lib.bin/em_m2
		-cmp $(SRC_DIR)/m2mm.1 $(TARGET_HOME)/man/m2mm.1

opr:
		make pr | opr

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/proto.main \
			$(SRC_DIR)/char.tab $(SRC_G) $(SRC_H) $(SRC_C)

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

make_macros:    Makefile
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
		rm -f $(GEN_C) $(GEN_G) $(GEN_H) LLfiles Cfiles LL.output
		rm -f resolved *.dep lists make.main make_macros

LLfiles:	$(GFILES)
		$(LLGEN) $(LLGENOPTIONS) $(GFILES)
		@touch LLfiles

tokenfile.g:	$(SRC_DIR)/tokenname.c $(SRC_DIR)/make.tokfile
		$(SRC_DIR)/make.tokfile <$(SRC_DIR)/tokenname.c >tokenfile.g

symbol2str.c:	$(SRC_DIR)/tokenname.c $(SRC_DIR)/make.tokcase
		$(SRC_DIR)/make.tokcase <$(SRC_DIR)/tokenname.c >symbol2str.c

char.c:		$(SRC_DIR)/char.tab
		$(TABGEN) -f$(SRC_DIR)/char.tab >char.c
