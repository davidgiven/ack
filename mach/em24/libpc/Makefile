MAKEFILE=../../proto/libg/Makefile
MACHDEF="MACH=int24" "SUF=m"
PCDEF="PREF=pc" "SUB=" "SRC=lang/pc/libpc"
LIBDIR=../lib

install:
	make -f $(MAKEFILE) $(PCDEF) $(MACHDEF) cp

cmp:
	make -f $(MAKEFILE) $(PCDEF) $(MACHDEF) all
	cmp head_pc $(LIBDIR)/head_pc
	cmp tail_pc $(LIBDIR)/tail_pc

clean:
		-rm -f *.old *.[ce$(SUF)] tail* head*

opr:
	make pr | opr

pr:
	@pr Makefile
