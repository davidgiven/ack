SUF=o
EMHOME=../../..
MAKEFILE=Makefile
MACHDEF="MACH=vax4" "SUF=$(SUF)" "ASAR=aal" "DEFS=-DBFS"
PREF=ac
SUB=
LIBC="PREF=$(PREF)" "SUB=$(SUB)"
OLIB=tail_$(PREF)$(SUB)

install: cplibc

cplibc:
	cp $(EMHOME)/lang/cem/libcc.ansi/libsrc .
	tar xf libsrc
	make -f $(MAKEFILE) $(LIBC) $(MACHDEF) "EMHOME=../$(EMHOME)" tailcp

cmp:	cmplibc
	-../../compare $(OLIB)

cmplibc:
	cp $(EMHOME)/lang/cem/libcc.ansi/libsrc .
	tar xf libsrc
	make -f $(MAKEFILE) $(LIBC) $(MACHDEF) "EMHOME=../$(EMHOME)" tail

clean:
	rm -rf `cat LIST` LIST OLIST MakeArch Makefile make.proto libsrc

opr:
	make pr | opr

pr:
	@pr makefile
