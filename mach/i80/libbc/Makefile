SUF=o
MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=i80" "SUF=$(SUF)" "ASAR=aal"
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
