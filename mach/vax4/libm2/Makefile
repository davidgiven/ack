SUF=o
EMHOME=../../..
MAKEFILE=$(EMHOME)/mach/proto/libg/Makefile
MACHDEF="MACH=vax4" "SUF=$(SUF)" "ASAR=ar"
M2LIB = lang/m2/libm2
MOD="PREF=m2" "SUB=" "SRC=$(M2LIB)"

all:
		make -f $(MAKEFILE) $(MOD) $(MACHDEF) all

install:	cpmod

cpmod:
		RANLIB=ranlib ; export RANLIB ; \
		make -f $(MAKEFILE) $(MOD) $(MACHDEF) cp

cmp:		cmpmod

cmpmod:
		RANLIB=ranlib ; export RANLIB ; \
		make -f $(MAKEFILE) $(MOD) $(MACHDEF) all
		-$(EMHOME)/mach/compare tail_m2
		-$(EMHOME)/mach/compare head_m2

clean:
		-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
		make pr | opr

pr:
		@pr Makefile
