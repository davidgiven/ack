# $Header$

#PARAMS		do not remove this line!

INSTALLDIR=$(TARGET_HOME)/bin
LIBDIR=$(TARGET_HOME)/lib/LLgen

all:
	cd src; make LIBDIR=$(LIBDIR)

clean:
	-cd src; make clean

install:
	cd src; make LIBDIR=$(LIBDIR)
	cp src/LLgen $(INSTALLDIR)/LLgen
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	cp $(SRC_HOME)/util/LLgen/lib/rec $(LIBDIR)/rec ; \
		cp $(SRC_HOME)/util/LLgen/lib/incl $(LIBDIR)/incl ; \
		mk_manpage $(SRC_HOME)/util/LLgen/LLgen.1 $(TARGET_HOME) ; \
	fi

firstinstall:
	cd src; make LIBDIR=$(LIBDIR) first
	cp src/LLgen $(INSTALLDIR)/LLgen
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	if [ -d $(LIBDIR) ] ; then : ; else mkdir $(LIBDIR) ; fi ; \
		cp $(SRC_HOME)/util/LLgen/lib/rec $(LIBDIR)/rec ; \
		cp $(SRC_HOME)/util/LLgen/lib/incl $(LIBDIR)/incl ; \
		mk_manpage $(SRC_HOME)/util/LLgen/LLgen.1 $(TARGET_HOME) ; \
	fi

cmp:
	cd src; make LIBDIR=$(LIBDIR)
	-cmp src/LLgen $(INSTALLDIR)/LLgen
	-cmp $(SRC_HOME)/util/LLgen/lib/rec $(LIBDIR)/rec
	-cmp $(SRC_HOME)/util/LLgen/lib/incl $(LIBDIR)/incl

distr:
	cd src; make distr

opr:
	make pr | opr

pr:
	@cd src; make pr
