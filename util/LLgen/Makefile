# $Header$
EMHOME=../..
INSTALLDIR=$(EMHOME)/bin
LIBDIR=$(EMHOME)/lib/LLgen
MANDIR=$(EMHOME)/man

all:
	cd src; make

clean:
	-cd src; make clean

install:
	cd src; make
	rm -f $(INSTALLDIR)/LLgen $(LIBDIR)/rec $(LIBDIR)/incl $(MANDIR)/LLgen.1
	cp src/LLgen $(INSTALLDIR)/LLgen
	cp lib/rec $(LIBDIR)/rec
	cp lib/incl $(LIBDIR)/incl
	cp LLgen.1 $(MANDIR)/LLgen.1

firstinstall:
	cd src; make first
	rm -f $(INSTALLDIR)/LLgen $(LIBDIR)/rec $(LIBDIR)/incl $(MANDIR)/LLgen.1
	cp src/LLgen $(INSTALLDIR)/LLgen
	-mkdir $(LIBDIR)
	cp lib/rec $(LIBDIR)/rec
	cp lib/incl $(LIBDIR)/incl
	cp LLgen.1 $(MANDIR)/LLgen.1

cmp:
	cd src; make
	-cmp src/LLgen $(INSTALLDIR)/LLgen
	-cmp lib/rec $(LIBDIR)/rec
	-cmp lib/incl $(LIBDIR)/incl
	-cmp LLgen.1 $(MANDIR)/LLgen.1

distr:
	cd src; make distr

opr:
	make pr | opr

pr:
	@cd src; make pr
