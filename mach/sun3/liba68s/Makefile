SUF=o
MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=sun3" "SUF=$(SUF)" "ASAR=aal"
A68SDEF="PREF=a68s" "SUB=" "SRC=lang/a68s/liba68s"

all:
	make -f $(MAKEFILE) $(A68SDEF) $(MACHDEF) tail

install:
	make -f $(MAKEFILE) $(A68SDEF) $(MACHDEF) tailcp

cmp:	all
	-../../compare tail_a68s

clean:
		-rm -f *.old *.[cehsp$(SUF)] tail* head* check??

opr:
	make pr | opr

pr:
	@pr Makefile
