SUF=o
MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=sun3" "SUF=$(SUF)" "ASAR=aal"
STDIO="PREF=cc" "SUB=.1s" "SRC=lang/cem/libcc/stdio"
GEN="PREF=cc" "SUB=.2g" "SRC=lang/cem/libcc/gen"
MON="PREF=mon" "SRC=lang/cem/libcc/mon"

install:	cpstdio cpgen

cpstdio:
		make -f $(MAKEFILE) $(STDIO) $(MACHDEF) tailcp
cpgen:
		make -f $(MAKEFILE) $(GEN) $(MACHDEF) cp
cpmon:
		make -f $(MAKEFILE) $(MON) $(MACHDEF) tailcp

cmp:		cmpstdio cmpgen

cmpstdio:
		make -f $(MAKEFILE) $(STDIO) $(MACHDEF) tail
		-../../compare tail_cc.1s
cmpgen:
		make -f $(MAKEFILE) $(GEN) $(MACHDEF) head
		-../../compare head_cc
		make -f $(MAKEFILE) $(GEN) $(MACHDEF) tail
		-../../compare tail_cc.2g
cmpmon:
		make -f $(MAKEFILE) $(MON) $(MACHDEF) tail
		-../../compare tail_mon

clean:
		-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
		make pr | opr

pr:
		@pr Makefile
