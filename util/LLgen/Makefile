# $Id$
INSTALLDIR=/usr/local/bin
LIBDIR=/usr/local/lib/LLgen
MANDIR=/usr/local/man/man1

all:
	cd src; make LIBDIR=$(LIBDIR)

clean:
	-cd src; make clean

install:
	cd src; make LIBDIR=$(LIBDIR)
	rm -f $(INSTALLDIR)/LLgen $(LIBDIR)/rec $(LIBDIR)/incl $(MANDIR)/LLgen.1
	cp src/LLgen $(INSTALLDIR)/LLgen
	cp lib/rec $(LIBDIR)/rec
	cp lib/incl $(LIBDIR)/incl
	cp lib/nc_rec $(LIBDIR)/nc_rec
	cp lib/nc_incl $(LIBDIR)/nc_incl
	cp LLgen.1 $(MANDIR)/LLgen.1

firstinstall:
	cd src; make LIBDIR=$(LIBDIR) first
	rm -f $(INSTALLDIR)/LLgen $(LIBDIR)/rec $(LIBDIR)/incl $(MANDIR)/LLgen.1
	cp src/LLgen $(INSTALLDIR)/LLgen
	-mkdir $(LIBDIR)
	cp lib/rec $(LIBDIR)/rec
	cp lib/incl $(LIBDIR)/incl
	cp lib/nc_rec $(LIBDIR)/nc_rec
	cp lib/nc_incl $(LIBDIR)/nc_incl
	cp LLgen.1 $(MANDIR)/LLgen.1

cmp:
	cd src; make LIBDIR=$(LIBDIR)
	-cmp src/LLgen $(INSTALLDIR)/LLgen
	-cmp lib/rec $(LIBDIR)/rec
	-cmp lib/incl $(LIBDIR)/incl
	-cmp lib/nc_rec $(LIBDIR)/nc_rec
	-cmp lib/nc_incl $(LIBDIR)/nc_incl
	-cmp LLgen.1 $(MANDIR)/LLgen.1

distr:
	cd src; make distr

opr:
	make pr | opr

pr:
	@cd src; make pr
