MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=m68k4" "SUF=o" "ASAR=aal"
BCDEF="PREF=bc" "SUB=" "SRC=lang/basic/lib"

install:
	make -f $(MAKEFILE) $(BCDEF) $(MACHDEF) tailcp

cmp:
	make -f $(MAKEFILE) $(BCDEF) $(MACHDEF) tail
	-../../compare head_bc
	-../../compare tail_bc

clean:
		-rm -f *.old *.[ceo] tail* head*

opr:
	make pr | opr

pr:
	@pr Makefile
