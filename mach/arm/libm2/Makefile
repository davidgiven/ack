SUF=so
MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=arm" "SUF=$(SUF)"
PCDEF="PREF=m2" "SUB=" "SRC=lang/m2/libm2"

install:
	make -f $(MAKEFILE) $(PCDEF) $(MACHDEF) cp


cmp:
	make -f $(MAKEFILE) $(PCDEF) $(MACHDEF) all
	-../../compare head_m2
	-../../compare tail_m2

clean:
		-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
	make pr | opr

pr:
	@pr Makefile
