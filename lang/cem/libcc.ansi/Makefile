# $Header$
# This Makefile is used for building the source archive as well as the
# actual library.

EMHOME=../../..
MACH=acc
MACHFL=-c.$(SUF) -O -L -Rcem=$(EMHOME)/lang/cem/libcc.new/main -I../headers \
	    $(DEFS)
PREF=ancc
SUB=
ASAR=arch
HEADSRC=$(HOME)
OLIB=tail_$(PREF)$(SUB)

libsrc:
	MakeArch

tail:	$(OLIB)

tailcp: tail
	../../install $(OLIB)
	rm -f $(OLIB)

$(OLIB):
	@echo translation test


libc:
	MACH=$(MACH) MACHFL="$(MACHFL) -LIB" ASAR=$(ASAR) \
		OLIB=$(OLIB) MakeArch -o

$(ASAR):
	@rm -f OLIST
	@sed 's/\.[ce]/.$(SUF)/' < LIST | sed '/\.h/D' > OLIST
	make "CC=$(MACH)" "MACHFL=$(MACHFL)" `cat OLIST`

# arch doesn't work recursively, tar does, which is what we actually want
tar:
	@rm -f OLIST
	make `cat LIST`
	cp LIST OLIST
