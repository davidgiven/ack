MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=z8000" "SUF=s"
BCDEF="PREF=bc" "SUB=" "SRC=lang/basic/lib"

install:
	make -f $(MAKEFILE) $(BCDEF) $(MACHDEF) tailcp

cmp:
	make -f $(MAKEFILE) $(BCDEF) $(MACHDEF) tail
	-../../compare head_bc
	-../../compare tail_bc

clean:
		-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
	make pr | opr

pr:
	@pr Makefile
