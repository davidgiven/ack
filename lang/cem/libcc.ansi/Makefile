# $Header$
# This Makefile is used for building the source archive as well as the
# actual library.

EMHOME=../../..
MACH=acc
MACHFL=-O -L -Rcem=$(EMHOME)/lib/em_cemcom.ansi -I../headers $(DEFS)
PREF=ac
SUB=
ASAR=arch
HEADSRC=$(HOME)
OLIB=tail_$(PREF)$(SUB)

install:
	MakeArch
	-mkdir $(EMHOME)/include/tail_ac
	( cd headers; tar cf - `cat LIST` ) | ( cd $(EMHOME)/include/tail_ac ; tar xf - )

clean:
	rm -f OLIST */OLIST
	(cd ctype; make clean)
	(cd stdlib; make clean)

headcp:	head
	../../install head_$(PREF).$(SUF) head_$(PREF)
	rm -f head_$(PREF).$(SUF)

head:	head_$(PREF).e
	make -r -f make.proto "CC=$(MACH)" "CFLAGS=-I$(EMHOME)/h $(MACHFL)" head_$(PREF).$(SUF)

tail:	lib

tailcp: tail
	../../install $(OLIB)
	rm -f $(OLIB)

lib:
	MACH=$(MACH) MACHFL="$(MACHFL) -LIB" ASAR=$(ASAR) SUF=$(SUF) \
		OLIB=$(OLIB) MakeArch -o

ar aal arch:
	@rm -f OLIST
	@sed 's/\.[ce]/.$(SUF)/' < LIST | sed '/\.h/D' > OLIST
	make -r -f ../make.proto "CC=$(MACH)" "CFLAGS=$(MACHFL)" `cat OLIST`

# arch doesn't work recursively, tar does, which is what we actually want
tar:
	@rm -f OLIST
	make `cat LIST`
	cp LIST OLIST
