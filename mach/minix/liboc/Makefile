SUF=o
MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=minix" "SUF=$(SUF)" "ASAR=aal"
PCDEF="PREF=ocm" "SUB=" "SRC=lang/occam/lib"

all:
	make -f $(MAKEFILE) $(PCDEF) $(MACHDEF) tail

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
