EMHOME = ../../..
CEG = $(EMHOME)/lib/ceg/util

all:
	make -f $(CEG)/make_obj

install:
	make -f $(CEG)/make_obj install

cmp:
	-make -f $(CEG)/make_obj cmp

pr:
	@pr Makefile EM_table mach.h mach.c as_table as.h as.c

opr:
	make pr | opr

# total cleanup
clean:
	make -f $(CEG)/make_obj clean

# only remove ce, ceg, and back directories
dclean:
	make -f $(CEG)/make_obj dclean
