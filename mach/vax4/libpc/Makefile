SUF=o
MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=vax4" "SUF=$(SUF)" "ASAR=ar"
PCDEF="PREF=pc" "SUB=" "SRC=lang/pc/libpc"

install:
	RANLIB=ranlib ; export RANLIB ;\
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
