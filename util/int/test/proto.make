# $Header$

#PARAMS		do not remove this line

.SUFFIXES:	.em22 .em24 .em44

.c.em22:
	$(UTIL_HOME)/bin/em22 $*.c -o $*.em22

.p.em22:
	$(UTIL_HOME)/bin/em22 $*.p -o $*.em22

.c.em24:
	$(UTIL_HOME)/bin/em24 $*.c -o $*.em24

.p.em24:
	$(UTIL_HOME)/bin/em24 $*.p -o $*.em24

.c.em44:
	$(UTIL_HOME)/bin/em44 $*.c -o $*.em44

.p.em44:
	$(UTIL_HOME)/bin/em44 $*.p -o $*.em44

clean:
	rm -f e.out core mon.out int.mess int.log int.core int.tally \
		*.k *.m *.o *.s *.em?? a.out

all install cmp pr opr:
