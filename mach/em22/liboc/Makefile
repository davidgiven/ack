SUF=m
MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=int22" "SUF=$(SUF)"
PCDEF="PREF=ocm" "SUB=" "SRC=lang/occam/lib"

install:
	make -f $(MAKEFILE) $(PCDEF) $(MACHDEF) tailcp

cmp:
	make -f $(MAKEFILE) $(PCDEF) $(MACHDEF) tail
	-../../compare tail_ocm

clean:
		-rm -f *.old *.[ce$(SUF)] tail*

opr:
	make pr | opr

pr:
	@pr Makefile
