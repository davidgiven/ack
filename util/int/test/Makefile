# $Header$

EM =		../../..#	# EM tree

.SUFFIXES:	.em22 .em24 .em44

.c.em22:
	$(EM)/bin/em22 $*.c -o $*.em22

.p.em22:
	$(EM)/bin/em22 $*.p -o $*.em22

.c.em24:
	$(EM)/bin/em24 $*.c -o $*.em24

.p.em24:
	$(EM)/bin/em24 $*.p -o $*.em24

.c.em44:
	$(EM)/bin/em44 $*.c -o $*.em44

.p.em44:
	$(EM)/bin/em44 $*.p -o $*.em44

clean:
	rm -f e.out core mon.out int.mess int.log int.core int.tally \
		*.k *.m *.o *.s *.em?? a.out

distr:	.distr

.distr:
	echo *.[cp] Makefile READ_ME | tr ' ' '\012' > .distr

all install cmp pr opr:
