# $Header$

# make C compiler

#PARAMS		do not remove this line!

UTIL_BIN = \
	$(UTIL_HOME)/bin
SRC_DIR = \
	$(SRC_HOME)/lang/cem/cemcom

TABGEN=	$(UTIL_BIN)/tabgen
LLGEN =	$(UTIL_BIN)/LLgen
LLGENOPTIONS = \
	-v

SRC_G =	$(SRC_DIR)/program.g $(SRC_DIR)/declar.g \
	$(SRC_DIR)/expression.g $(SRC_DIR)/statement.g $(SRC_DIR)/ival.g
GEN_G =	tokenfile.g
GFILES=	$(GEN_G) $(SRC_G)

SRC_C = \
	$(SRC_DIR)/Version.c \
	$(SRC_DIR)/LLlex.c \
	$(SRC_DIR)/LLmessage.c \
	$(SRC_DIR)/arith.c \
	$(SRC_DIR)/blocks.c \
	$(SRC_DIR)/ch7.c \
	$(SRC_DIR)/ch7bin.c \
	$(SRC_DIR)/ch7mon.c \
	$(SRC_DIR)/code.c \
	$(SRC_DIR)/conversion.c \
	$(SRC_DIR)/cstoper.c \
	$(SRC_DIR)/dataflow.c \
	$(SRC_DIR)/declarator.c \
	$(SRC_DIR)/decspecs.c \
	$(SRC_DIR)/domacro.c \
	$(SRC_DIR)/dumpidf.c \
	$(SRC_DIR)/error.c \
	$(SRC_DIR)/eval.c \
	$(SRC_DIR)/expr.c \
	$(SRC_DIR)/field.c \
	$(SRC_DIR)/idf.c \
	$(SRC_DIR)/init.c \
	$(SRC_DIR)/input.c \
	$(SRC_DIR)/l_comment.c \
	$(SRC_DIR)/l_ev_ord.c \
	$(SRC_DIR)/l_lint.c \
	$(SRC_DIR)/l_misc.c \
	$(SRC_DIR)/l_outdef.c \
	$(SRC_DIR)/l_states.c \
	$(SRC_DIR)/label.c \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/options.c \
	$(SRC_DIR)/replace.c \
	$(SRC_DIR)/scan.c \
	$(SRC_DIR)/skip.c \
	$(SRC_DIR)/stack.c \
	$(SRC_DIR)/struct.c \
	$(SRC_DIR)/switch.c \
	$(SRC_DIR)/tokenname.c \
	$(SRC_DIR)/type.c \
	$(SRC_DIR)/util.c
GEN_C =	tokenfile.c program.c declar.c expression.c statement.c ival.c \
	symbol2str.c char.c Lpars.c next.c
CFILES=	$(SRC_C) $(GEN_C)

SRC_H = \
	$(SRC_DIR)/LLlex.h \
	$(SRC_DIR)/align.h \
	$(SRC_DIR)/arith.h \
	$(SRC_DIR)/assert.h \
	$(SRC_DIR)/atw.h \
	$(SRC_DIR)/class.h \
	$(SRC_DIR)/decspecs.h \
	$(SRC_DIR)/file_info.h \
	$(SRC_DIR)/input.h \
	$(SRC_DIR)/interface.h \
	$(SRC_DIR)/l_class.h \
	$(SRC_DIR)/l_comment.h \
	$(SRC_DIR)/l_em.h \
	$(SRC_DIR)/l_lint.h \
	$(SRC_DIR)/label.h \
	$(SRC_DIR)/level.h \
	$(SRC_DIR)/mes.h \
	$(SRC_DIR)/sizes.h \
	$(SRC_DIR)/specials.h \
	$(SRC_DIR)/tokenname.h

GEN_H = botch_free.h dataflow.h debug.h density.h errout.h \
        idfsize.h ifdepth.h inputtype.h lint.h \
        nobitfield.h nopp.h nocross.h \
        nparams.h numsize.h parbufsize.h pathlength.h Lpars.h \
        strsize.h target_sizes.h textsize.h use_tmp.h spec_arith.h static.h \
        regcount.h \
	code.h declar.h decspecs.h def.h expr.h field.h estack.h util.h \
	idf.h macro.h stmt.h struct.h switch.h type.h l_brace.h l_state.h \
	l_outdef.h stack.h

HFILES=	$(GEN_H) $(SRC_H)

NEXTFILES = \
	$(SRC_DIR)/code.str \
	$(SRC_DIR)/declar.str \
	$(SRC_DIR)/decspecs.str \
	$(SRC_DIR)/def.str \
	$(SRC_DIR)/expr.str \
	$(SRC_DIR)/field.str \
        $(SRC_DIR)/estack.str \
	$(SRC_DIR)/util.str \
        $(SRC_DIR)/idf.str \
	$(SRC_DIR)/macro.str \
	$(SRC_DIR)/stack.str \
	$(SRC_DIR)/stmt.str \
	$(SRC_DIR)/struct.str \
	$(SRC_DIR)/switch.str \
	$(SRC_DIR)/type.str \
        $(SRC_DIR)/l_brace.str \
	$(SRC_DIR)/l_state.str \
	$(SRC_DIR)/l_outdef.str

all:		make.main
		make -f make.main main

install:	all
		@-mkdir $(TARGET_HOME)
		@-mkdir $(TARGET_HOME)/lib.bin
		@-mkdir $(TARGET_HOME)/man
		cp main $(TARGET_HOME)/lib.bin/em_cemcom
		cp $(SRC_HOME)/cemcom.1 $(TARGET_HOME)/man/em_cemcom.6

cmp:		all
		-cmp main $(TARGET_HOME)/lib.bin/em_cemcom
		-cmp $(SRC_HOME)/cemcom.1 $(TARGET_HOME)/man/em_cemcom.6

opr:
		make pr | opr

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/proto.main Parameters \
			$(SRC_DIR)/char.tab $(SRC_G) $(SRC_H) $(NEXTFILES) $(SRC_C)

lint:		make.main
		make -f make.main lint

Cfiles:		hfiles LLfiles $(GEN_C) $(GEN_H)
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

code.h:		$(SRC_DIR)/code.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/code.str > code.h

declar.h:	$(SRC_DIR)/declar.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/declar.str > declar.h

def.h:	$(SRC_DIR)/def.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/def.str > def.h

expr.h:	$(SRC_DIR)/expr.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/expr.str > expr.h

field.h:	$(SRC_DIR)/field.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/field.str > field.h

estack.h:	$(SRC_DIR)/estack.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/estack.str > estack.h

util.h:	$(SRC_DIR)/util.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/util.str > util.h

decspecs.h:	$(SRC_DIR)/decspecs.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/decspecs.str > decspecs.h

idf.h:	$(SRC_DIR)/idf.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/idf.str > idf.h

macro.h:	$(SRC_DIR)/macro.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/macro.str > macro.h

stack.h:	$(SRC_DIR)/stack.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/stack.str > stack.h

stmt.h:	$(SRC_DIR)/stmt.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/stmt.str > stmt.h

struct.h:	$(SRC_DIR)/struct.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/struct.str > struct.h

switch.h:	$(SRC_DIR)/switch.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/switch.str > switch.h

type.h:	$(SRC_DIR)/type.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/type.str > type.h

l_brace.h:	$(SRC_DIR)/l_brace.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/l_brace.str > l_brace.h

l_state.h:	$(SRC_DIR)/l_state.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/l_state.str > l_state.h

l_outdef.h:	$(SRC_DIR)/l_outdef.str $(SRC_DIR)/make.allocd
		$(SRC_DIR)/make.allocd < $(SRC_DIR)/l_outdef.str > l_outdef.h

next.c:		$(NEXTFILES) $(SRC_DIR)/make.next
		$(SRC_DIR)/make.next $(NEXTFILES) > next.c

char.c:		$(SRC_DIR)/char.tab
		$(TABGEN) -f$(SRC_DIR)/char.tab >char.c
