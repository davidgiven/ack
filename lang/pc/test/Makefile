# $Header$

FLOATS=
# for machines without floating point:
FLOATS=-DNOFLOAT
# for machines with software floating point library:
FLOATS=-fp
PC = apc

all:            testC testI

testI:
		em22 $(FLOATS) t1.p; int
		em22 $(FLOATS) t2.p; int
		em22 $(FLOATS) t3.p; int e.out f1 f2 f3 f4 f5 f6
		em22 $(FLOATS) t4.p; int
		em22 $(FLOATS) t5.p; int
		em22 $(FLOATS) tstenc.p; int
		em22 $(FLOATS) tstgto.p; int
		em22 $(FLOATS) -.p callc.p cmod.c ; int
		rm -f e.out f? *.k int.log int.mess

testC:
		$(PC) $(FLOATS) t1.p ; a.out
		$(PC) $(FLOATS) t2.p ; a.out
		$(PC) $(FLOATS) t3.p ; a.out f1 f2 f3 f4 f5 f6
		$(PC) $(FLOATS) t4.p ; a.out
		$(PC) $(FLOATS) t5.p ; a.out
		$(PC) $(FLOATS) tstenc.p ; a.out
		$(PC) $(FLOATS) tstgto.p ; a.out
		$(PC) $(FLOATS) callc.p cmod.c ; a.out
		rm -f a.out f? *.[os]

install cmp:

clean:
		-rm -f [ea].out f? *.o

opr:
		make pr | opr

pr:
		@pr t[12345].p tstenc.p tstgto.p callc.p cmod.c
