EMHOME = ../../..
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare

all:

install:	all
		$(INSTALL) pkg/idf_pkg.body
		$(INSTALL) pkg/idf_pkg.spec
		$(INSTALL) man/idf.3

cmp:		all
		-$(COMPARE) pkg/idf_pkg.body
		-$(COMPARE) pkg/idf_pkg.spec
		-$(COMPARE) man/idf.3

pr:
		@pr Makefile idf_pkg.spec idf_pkg.body

opr:
		make pr | opr

clean:
