SUF=o
EMHOME=../../..
MAKEFILE=Makefile
MACHDEF="MACH=sun3" "SUF=$(SUF)" "ASAR=aal" "DEFS=-DNFS"
PREF=ac
SUB=
LIBC="PREF=$(PREF)" "SUB=$(SUB)"
OLIB=tail_$(PREF)$(SUB)

install: cplibc cphead

cplibc: libsrc
	make -f $(MAKEFILE) $(LIBC) $(MACHDEF) "EMHOME=`homefromsubdir $(EMHOME)`" tailcp

cphead: head_$(PREF).e
	make -f $(MAKEFILE) $(LIBC) $(MACHDEF) "EMHOME=$(EMHOME)" headcp

head_$(PREF).e: libsrc

libsrc: $(EMHOME)/lang/cem/libcc.ansi/libsrc .
	cp $(EMHOME)/lang/cem/libcc.ansi/libsrc .
	tar xf libsrc

cmp:	cmplibc cmphead
	-../../compare $(OLIB)

cmplibc: libsrc
	make -f $(MAKEFILE) $(LIBC) $(MACHDEF) "EMHOME=`homefromsubdir $(EMHOME)`" tail

cmphead: head_$(PREF).e
	make -f $(MAKEFILE) $(LIBC) $(MACHDEF) "EMHOME=$(EMHOME)" head

clean:
	rm -rf `cat LIST` LIST OLIST MakeArch Makefile make.proto libsrc head_$(PREF).[e$(SUF)]

opr:
	make pr | opr

pr:
	@pr makefile
