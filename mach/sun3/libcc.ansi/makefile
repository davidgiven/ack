SUF=o
EMHOME=../../..
MAKEFILE=Makefile
MACHDEF="MACH=sun3" "SUF=$(SUF)" "ASAR=aal" "DEFS=-DNFS"
LIBC="PREF=ac"
OLIB=tail_$(PREF)$(SUB)

libc:
	cp $(EMHOME)/lang/cem/libcc.ansi/libsrc .
	tar xf libsrc
	make -f $(MAKEFILE) $(LIBC) $(MACHDEF) "EMHOME=../$(EMHOME)" tailcp

cmp:	cmplibc


cmplibc:
	make -f $(MAKEFILE) $(LIBC) $(MACHDEF) tail

clean:
	rm -rf `cat LIST` LIST MakeArch Makefile make.proto libsrc
