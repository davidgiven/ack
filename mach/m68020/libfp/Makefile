EMHOME=../../..
SUF=o
MAKEFILE=$(EMHOME)/mach/proto/fp/Makefile
MACHDEF="MACH=m68020" "CFLAGS=-DIEEEFORMAT -DUSE_DIVIDE" "SUF=$(SUF)" "ASAR=aal"

all:
	make -f $(MAKEFILE) $(MACHDEF) all

install:
	make -f $(MAKEFILE) $(MACHDEF) install

cmp:	all
	-../../compare FP_$(MACH).a tail_fp

clean:
	make -f $(MAKEFILE) $(MACHDEF) clean

opr:
	make pr | opr

pr:
	@pr Makefile
