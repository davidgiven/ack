# $Header$

# make ANSI C pre-processor

#PARAMS		do not remove this line!

UTIL_BIN = \
	$(UTIL_HOME)/bin
SRC_DIR = \
	$(SRC_HOME)/lang/cem/cpp.ansi

TABGEN=	$(UTIL_BIN)/tabgen
LLGEN =	$(UTIL_BIN)/LLgen
LLGENOPTIONS = \
	-v

SRC_G =	$(SRC_DIR)/expression.g
GEN_G =	tokenfile.g
GFILES=	$(GEN_G) $(SRC_G)

SRC_C = \
	$(SRC_DIR)/LLlex.c \
	$(SRC_DIR)/LLmessage.c \
	$(SRC_DIR)/ch3bin.c \
	$(SRC_DIR)/ch3mon.c \
	$(SRC_DIR)/domacro.c \
	$(SRC_DIR)/error.c \
	$(SRC_DIR)/idf.c \
	$(SRC_DIR)/init.c \
	$(SRC_DIR)/input.c \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/options.c \
	$(SRC_DIR)/preprocess.c \
	$(SRC_DIR)/replace.c \
	$(SRC_DIR)/skip.c \
	$(SRC_DIR)/tokenname.c \
	$(SRC_DIR)/expr.c

GEN_C =	tokenfile.c expression.c symbol2str.c char.c Lpars.c next.c
CFILES=	$(SRC_C) $(GEN_C)

SRC_H = \
	$(SRC_DIR)/LLlex.h \
	$(SRC_DIR)/bits.h \
	$(SRC_DIR)/file_info.h \
	$(SRC_DIR)/idf.h \
	$(SRC_DIR)/input.h \
	$(SRC_DIR)/interface.h \
	$(SRC_DIR)/class.h

GEN_H = botch_free.h debug.h errout.h obufsize.h dobits.h \
        idfsize.h ifdepth.h inputtype.h macbuf.h ln_prefix.h \
        nparams.h numsize.h parbufsize.h pathlength.h Lpars.h \
        strsize.h textsize.h macro.h replace.h

HFILES=	$(GEN_H) $(SRC_H)

NEXTFILES = \
	$(SRC_DIR)/macro.str \
	$(SRC_DIR)/replace.str

all:		make.main
		make -f make.main main

install:	all
		cp main $(TARGET_HOME)/lib.bin/cpp.ansi
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	cp $(SRC_DIR)/ncpp.6 cpp.ansi.6 ; \
			mk_manpage cpp.ansi.6 $(TARGET_HOME) ; \
			rm -f cpp.ansi.6 ; \
		fi

cmp:		all
		-cmp main $(TARGET_HOME)/lib.bin/cpp.ansi

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
		echo 'LINTSUF=$(LINTSUF)' >> make_macros
		echo 'LINTPREF=$(LINTPREF)' >> make_macros
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
		rm -f $(GEN_C) $(GEN_G) $(GEN_H) hfiles LLfiles Cfiles LL.output
		rm -f resolved *.dep lists make.main make_macros

LLfiles:	$(GFILES)
		$(LLGEN) $(LLGENOPTIONS) $(GFILES)
		@touch LLfiles

hfiles:		Parameters $(SRC_DIR)/make.hfiles
		$(SRC_DIR)/make.hfiles Parameters
		touch hfiles

tokenfile.g:	$(SRC_DIR)/tokenname.c $(SRC_DIR)/make.tokfile
		$(SRC_DIR)/make.tokfile <$(SRC_DIR)/tokenname.c >tokenfile.g

symbol2str.c:	$(SRC_DIR)/tokenname.c $(SRC_DIR)/make.tokcase
		$(SRC_DIR)/make.tokcase <$(SRC_DIR)/tokenname.c >symbol2str.c

replace.h:	$(SRC_DIR)/replace.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/replace.str > replace.h

macro.h:	$(SRC_DIR)/macro.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/macro.str > macro.h

next.c:		$(NEXTFILES) $(SRC_DIR)/make.next
		$(SRC_DIR)/make.next $(NEXTFILES) > next.c

char.c:		$(SRC_DIR)/char.tab
		$(TABGEN) -f$(SRC_DIR)/char.tab >char.c
