SUF=o
MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=ns" "SUF=$(SUF)" "ASAR=aal"
PCDEF="PREF=pc" "SUB=" "SRC=lang/pc/libpc"

install:
	make -f $(MAKEFILE) $(PCDEF) $(MACHDEF) cp

cmp:
	make -f $(MAKEFILE) $(PCDEF) $(MACHDEF) all
	-../../compare head_pc
	-../../compare tail_pc

clean:
	-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
	make pr | opr

pr:
	@pr Makefile
