SUF=m
MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=int22" "SUF=$(SUF)"
M2DEF="PREF=m2" "SUB=" "SRC=lang/m2/libm2"

all:
	make -f $(MAKEFILE) $(M2DEF) $(MACHDEF) all
	
install:
	make -f $(MAKEFILE) $(M2DEF) $(MACHDEF) cp

cmp:
	make -f $(MAKEFILE) $(M2DEF) $(MACHDEF) all
	-../../compare head_pc
	-../../compare tail_pc

clean:
		-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
	make pr | opr

pr:
	@pr Makefile
