MAKEFILE=Makef.pr.l
MACHDEF="MACH=z8000" "SUF=s"
STDIO="PREF=cc" "SUB=.1s" "SRC=lang/cem/libcc/stdio"
GEN="PREF=cc" "SUB=.2g" "SRC=lang/cem/libcc/gen"
MON="PREF=mon" "SRC=lang/cem/libcc/mon"
LIBDIR=../lib

install:	cpstdio cpgen cpmon

cpstdio:
		make -f $(MAKEFILE) $(STDIO) $(MACHDEF) tailcp
cpgen:
		make -f $(MAKEFILE) $(GEN) $(MACHDEF) cp
cpmon:
		make -f $(MAKEFILE) $(MON) $(MACHDEF) tailcp

cmp:		cmpstdio cmpgen cmpmon

cmpstdio:
		make -f $(MAKEFILE) $(STDIO) $(MACHDEF) tail
		-cmp tail_cc.1s $(LIBDIR)/tail_cc.1s
cmpgen:
		make -f $(MAKEFILE) $(GEN) $(MACHDEF) head
		-cmp head_cc $(LIBDIR)/head_cc
		make -f $(MAKEFILE) $(GEN) $(MACHDEF) tail
		-cmp tail_cc.2g $(LIBDIR)/tail_cc.2g
cmpmon:
		make -f $(MAKEFILE) $(MON) $(MACHDEF) tail
		-cmp tail_mon $(LIBDIR)/tail_mon

clean:
		-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
		make pr | opr

pr:
		@pr Makefile
