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
	rm -f $(INSTALLDIR)/LLgen $(LIBDIR)/rec $(LIBDIR)/incl $(MANDIR)/LLgen.1
	cp src/LLgen $(INSTALLDIR)/LLgen
	cp $(SRC_HOME)/util/LLgen/lib/rec $(LIBDIR)/rec
	cp $(SRC_HOME)/util/LLgen/lib/incl $(LIBDIR)/incl
	cp $(SRC_HOME)/util/LLgen/LLgen.1 $(MANDIR)/LLgen.1

firstinstall:
	cd src; make first
	rm -f $(INSTALLDIR)/LLgen $(LIBDIR)/rec $(LIBDIR)/incl $(MANDIR)/LLgen.1
	cp src/LLgen $(INSTALLDIR)/LLgen
	-mkdir $(LIBDIR)
	cp $(SRC_HOME)/util/LLgen/lib/rec $(LIBDIR)/rec
	cp $(SRC_HOME)/util/LLgen/lib/incl $(LIBDIR)/incl
	cp $(SRC_HOME)/util/LLgen/LLgen.1 $(MANDIR)/LLgen.1

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
