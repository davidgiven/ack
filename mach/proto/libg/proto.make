# $Header$

#PARAMS		do not remove this line!

MACHFL= -O -L
SUB =
ASAR = aal
RANLIB = :
COMPILE = compmodule
#PREF = pc
#SRC=lang/pc/libpc
#MACH=MACHINE
#SUF=o

HEADSRC=$(SRC_HOME)/$(SRC)/head_$(PREF).e

all:    head tail

head:   head_$(PREF)

tail:   tail_$(PREF)$(SUB)

headcp: head
	cp head_$(PREF) $(TARGET_HOME)/lib/$(MACH)/head_$(PREF)
	rm -f head_$(PREF)

tailcp: tail
	cp tail_$(PREF)$(SUB) $(TARGET_HOME)/lib/$(MACH)/tail_$(PREF)$(SUB)
	$(RANLIB) $(TARGET_HOME)/lib/$(MACH)/tail_$(PREF)$(SUB)
	rm -f tail_$(PREF)$(SUB)

cp:     headcp tailcp

head_$(PREF):  $(HEADSRC)
	cp $(HEADSRC) head_$(PREF).e
	$(MACH) $(MACHFL) -c -I$(TARGET_HOME)/h head_$(PREF).e
	mv head_$(PREF).$(SUF) head_$(PREF)
	-rm -f head_$(PREF).[ekm$(SUF)]

tail_$(PREF)$(SUB):
	@echo translation test
	@$(MACH) $(MACHFL) -c $(SRC_HOME)/mach/proto/libg/barrier.c
	@-rm -f barrier.[oeskm] barrier.so
	@echo OK
	-rm -f tail_$(PREF)$(SUB)
	MACH="$(MACH)" MACHFL="$(MACHFL) -LIB" ASAR=$(ASAR) SRC=$(SRC) \
		RANLIB="$(RANLIB)" SUF="$(SUF)" \
		march $(SRC_HOME)/$(SRC) tail_$(PREF)$(SUB) $(COMPILE)
