# $Header$

MACH=MACHINE
MACHFL=-c.$(SUF) -O -L
SUB =
PREF=pc
ASAR=arch
SRC=lang/pc/libpc
HOME = ../../..
HEADSRC=$(HOME)/$(SRC)/head_$(PREF).e

all:    head tail

head:   head_$(PREF) $(HOME)/h/*.h

tail:   tail_$(PREF)$(SUB) $(HOME)/h/*.h

headcp: head
	../../install head_$(PREF)
	rm -f head_$(PREF)

tailcp: tail
	../../install tail_$(PREF)$(SUB)
	rm -f tail_$(PREF)$(SUB)

cp:     headcp tailcp

head_$(PREF):  $(HEADSRC)
	cp $(HEADSRC) head_$(PREF).e
	$(MACH) $(MACHFL) head_$(PREF).e
	mv head_$(PREF).$(SUF) head_$(PREF)
	-rm -f head_$(PREF).[ekm$(SUF)]

tail_$(PREF)$(SUB):
	@echo translation test
	@$(MACH) $(MACHFL) $(HOME)/mach/proto/libg/barrier.c
	@-rm barrier.[oeskm]
	@echo OK
	-rm -f tail_$(PREF)$(SUB)
	MACH="$(MACH)" MACHFL="$(MACHFL)" ASAR=$(ASAR) \
		march $(HOME)/$(SRC) tail_$(PREF)$(SUB)
