EMHOME = ../../..
CEG = $(EMHOME)/lib/ceg/util
OBJ = obj

all:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ)

install:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) install

cmp:
	-make -f $(CEG)/make_asobj "OBJ="$(OBJ) cmp

pr:
	@pr Makefile EM_table mach.h mach.c as_table as.h as.c

opr:
	make pr | opr

# total cleanup
clean:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) clean

# only remove ce, ceg, and back directories
dclean:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) dclean
