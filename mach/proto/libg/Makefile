# $Header$

MACH=MACHINE
MACHFL=-c -O -L
SUB =
PREF=pc
ASAR=arch
SRC=lang/pc/libpc
HOME = ../../..
TAILSRC=$(HOME)/$(SRC)/tail_$(PREF)$(SUB).a
HEADSRC=$(HOME)/$(SRC)/head_$(PREF).e

all:    head tail

head:   head_$(PREF) $(HOME)/h/*.h

tail:   tail_$(PREF)$(SUB) $(HOME)/h/*.h

headcp: head
	cp head_$(PREF) ../lib/head_$(PREF)
	rm -f head_$(PREF)

tailcp: tail
	cp tail_$(PREF)$(SUB) ../lib/tail_$(PREF)$(SUB)
	-case $(ASAR) in \
	ar)	ranlib ../lib/tail_$(PREF)$(SUB) ;; \
	esac
	rm -f tail_$(PREF)$(SUB)

cp:     headcp tailcp

head_$(PREF):  $(HEADSRC)
	cp $(HEADSRC) head_$(PREF).e
	$(MACH) $(MACHFL) head_$(PREF).e
	mv head_$(PREF).$(SUF) head_$(PREF)
	-rm -f head_$(PREF).[ekm$(SUF)]

tail_$(PREF)$(SUB): $(TAILSRC)
	@echo translation test
	@$(MACH) $(MACHFL) $(HOME)/mach/proto/libg/barrier.c
	@-rm barrier.[oeskm]
	@echo OK
	-rm -f tail_$(PREF)$(SUB)
	arch x $(TAILSRC)
	-LIST= ;\
	for i in `arch t $(TAILSRC)`;\
	do if $(MACH) -I$(HOME)/h $(MACHFL) -LIB $$i ; then \
		case $$i in \
		*.c)    LIST="$$LIST `basename $$i .c`.$(SUF)" ;;\
		*.e)    LIST="$$LIST `basename $$i .e`.$(SUF)" ;;\
		*.p)    LIST="$$LIST `basename $$i .p`.$(SUF)" ;;\
	esac \
	else rm -f `basename $$i .c`.[k$(SUF)m] ;\
	fi ;\
	rm -f $$i ;\
	done ;\
	$(ASAR) r tail_$(PREF)$(SUB) $$LIST ; rm -f $$LIST
