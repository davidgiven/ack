SUF=o
EMHOME=../../..
MAKEFILE=$(EMHOME)/mach/proto/libg/Makefile
MACHDEF="MACH=z80" "SUF=$(SUF)" "ASAR=aal"
M2LIB = lang/m2/libm2
MOD="PREF=m2" "SUB=" "SRC=$(M2LIB)"

all:
		make -f $(MAKEFILE) $(MOD) $(MACHDEF) all

install:	cpmod

cpmod:
		make -f $(MAKEFILE) $(MOD) $(MACHDEF) cp

cmp:		cmpmod

cmpmod:
		make -f $(MAKEFILE) $(MOD) $(MACHDEF) all
		-$(EMHOME)/mach/compare tail_m2
		-$(EMHOME)/mach/compare head_m2

clean:
		-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
		make pr | opr

pr:
		@pr Makefile
