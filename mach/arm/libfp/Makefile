EMHOME=../../..
SUF=so
MAKEFILE=$(EMHOME)/mach/proto/fp/Makefile
MACHDEF="MACH=arm" "CFLAGS=-DUSE_DIVIDE" "SUF=$(SUF)" "ASAR=arch"

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
