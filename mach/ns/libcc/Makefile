MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=ns" "SUF=o"
STDIO="PREF=cc" "SUB=.1s" "SRC=lang/cem/libcc/stdio"
GEN="PREF=cc" "SUB=.2g" "SRC=lang/cem/libcc/gen"
MON="PREF=mon" "SRC=lang/cem/libcc/mon"
LIBM="PREF=m" "SRC=lang/cem/libcc/libm"
LIBLN="PREF=ln" "SRC=lang/cem/libcc/libln"

install:	cpstdio cpgen cpmon

cpstdio:
		make -f $(MAKEFILE) $(STDIO) $(MACHDEF) tailcp
cpgen:
		make -f $(MAKEFILE) $(GEN) $(MACHDEF) cp
cpmon:
		make -f $(MAKEFILE) $(MON) $(MACHDEF) tailcp
cplibm:
		make -f $(MAKEFILE) $(LIBM) $(MACHDEF) tailcp
cplibln:
		make -f $(MAKEFILE) $(LIBLN) $(MACHDEF) tailcp

cmp:		cmpstdio cmpgen cmpmon cmplib cmplibln

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
cmplibm:
		make -f $(MAKEFILE) $(LIBM) $(MACHDEF) tail
		-../../compare tail_m
cmplibln:
		make -f $(MAKEFILE) $(LIBLN) $(MACHDEF) tail
		-../../compare tail_ln

clean:
		-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
		make pr | opr

pr:
		@pr Makefile
