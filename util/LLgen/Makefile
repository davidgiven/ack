# $Header$
INSTALLDIR=../../bin
LIBDIR=../../lib/LLgen

all:	cmp

clean:
	-cd src; make clean

install:
	cd src; make
	cp src/LLgen $(INSTALLDIR)/LLgen
	cp lib/rec $(LIBDIR)/rec
	cp lib/incl $(LIBDIR)/incl

cmp:
	cd src; make
	-cmp src/LLgen $(INSTALLDIR)/LLgen
	-cmp lib/rec $(LIBDIR)/rec
	-cmp lib/incl $(LIBDIR)/incl

opr:
	make pr | opr

pr:
	@cd src; make pr
	@cd lib; make pr
