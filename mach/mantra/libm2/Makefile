SUF=o
EMHOME=../../..
MAKEFILE=$(EMHOME)/mach/proto/libg/Makefile
MACHDEF="MACH=mantra" "SUF=$(SUF)" "ASAR=aal"
M2LIB = lang/m2/libm2
MOD="PREF=m2" "SUB=" "SRC=$(M2LIB)"

install:	cpmod

cpmod:
		make -f $(MAKEFILE) $(MOD) $(MACHDEF) cp

cmp:		cmpmod

cmpmod:
		make -f $(MAKEFILE) $(MOD) $(MACHDEF) tail
		-$(EMHOME)/mach/compare tail_m2
		make -f $(MAKEFILE) $(MOD) $(MACHDEF) head
		-$(EMHOME)/mach/compare head_m2

clean:
		-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
		make pr | opr

pr:
		@pr Makefile
