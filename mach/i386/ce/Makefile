EMHOME = ../../..
CEG = $(EMHOME)/lib/ceg/util
VERSION = make_obj

all:
	make -f $(CEG)/$(VERSION)

install:
	make -f $(CEG)/$(VERSION) install

cmp:
	-make -f $(CEG)/$(VERSION) cmp

pr:
	@pr Makefile EM_table mach.h mach.c as_table as.h as.c

opr:
	make pr | opr

# total cleanup
clean:
	make -f $(CEG)/$(VERSION) clean

# only remove ce, ceg, and back directories
dclean:
	make -f $(CEG)/$(VERSION) dclean
