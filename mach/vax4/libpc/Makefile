SUF=o
MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=vax4" "SUF=$(SUF)" "ASAR=aal"
PCDEF="PREF=pc" "SUB=" "SRC=lang/pc/libpc"

all:
	make -f $(MAKEFILE) $(PCDEF) $(MACHDEF) all

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
