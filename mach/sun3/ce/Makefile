EMHOME=../../..
BACK=$(EMHOME)/lib/ceg/ce_back
CEG = $(EMHOME)/lib/ceg/util

all:	back.a
	make -f $(CEG)/make_own

install:	back.a
	make -f $(CEG)/make_own install

cmp:	back.a
	-make -f $(CEG)/make_own cmp

pr:
	@pr Makefile EM_table mach.h mach.c as_table as.h as.c \
	Make.back do_close.c do_open.c end_back.c misc.c output.c relocation.c

opr:
	make pr | opr

# total cleanup
clean:
	make -f $(CEG)/make_own clean
	rm -rf back back.a

# only remove ce, ceg, and back directories
dclean:
	make -f $(CEG)/make_own dclean
	rm -rf back

back.a :
	-mkdir back
	cp $(BACK)/obj_back/*h back
	cp Make.back back/Makefile
	cd back; make ; cd ..
	ar r back.a back/*o;
	-sh -c 'ranlib back.a'
