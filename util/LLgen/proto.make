# $Header$

#PARAMS		do not remove this line!

INSTALLDIR=$(TARGET_HOME)/bin
LIBDIR=$(TARGET_HOME)/lib/LLgen
MANDIR=$(TARGET_HOME)/man

all:
	cd src; make

clean:
	-cd src; make clean

install:
	cd src; make
	cp src/LLgen $(INSTALLDIR)/LLgen
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	cp $(SRC_HOME)/util/LLgen/lib/rec $(LIBDIR)/rec ; \
		cp $(SRC_HOME)/util/LLgen/lib/incl $(LIBDIR)/incl ; \
		cp $(SRC_HOME)/util/LLgen/LLgen.1 $(MANDIR)/LLgen.1 ; \
	fi

firstinstall:
	cd src; make first
	cp src/LLgen $(INSTALLDIR)/LLgen
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	if [ -d $(LIBDIR) ] ; then : ; else mkdir $(LIBDIR) ; fi ; \
		cp $(SRC_HOME)/util/LLgen/lib/rec $(LIBDIR)/rec ; \
		cp $(SRC_HOME)/util/LLgen/lib/incl $(LIBDIR)/incl ; \
		cp $(SRC_HOME)/util/LLgen/LLgen.1 $(MANDIR)/LLgen.1 ; \
	fi

cmp:
	cd src; make
	-cmp src/LLgen $(INSTALLDIR)/LLgen
	-cmp $(SRC_HOME)/util/LLgen/lib/rec $(LIBDIR)/rec
	-cmp $(SRC_HOME)/util/LLgen/lib/incl $(LIBDIR)/incl
	-cmp $(SRC_HOME)/util/LLgen/LLgen.1 $(MANDIR)/LLgen.1

distr:
	cd src; make distr

opr:
	make pr | opr

pr:
	@cd src; make pr
