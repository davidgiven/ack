# $Header$

NOFL =

all:            testC testI

testI:
		em22 $(NOFL) t1.p; int
		em22 $(NOFL) t2.p; int
		em22 $(NOFL) t3.p; int e.out f1 f2 f3 f4 f5 f6
		em22 $(NOFL) t4.p; int
		em22 $(NOFL) t5.p; int
		em22 $(NOFL) tstenc.p; int
		em22 $(NOFL) tstgto.p; int
		em22 $(NOFL) -.p callc.p cmod.c ; int
		rm -f e.out f? *.k

testC:
		apc $(NOFL) t1.p; a.out
		apc $(NOFL) t2.p; a.out
		apc $(NOFL) t3.p; a.out f1 f2 f3 f4 f5 f6
		apc $(NOFL) t4.p; a.out
		apc $(NOFL) t5.p; a.out
		apc $(NOFL) tstenc.p; a.out
		apc $(NOFL) tstgto.p; a.out
		apc $(NOFL) -.p callc.p cmod.c ; a.out
		rm -f a.out f? *.[os]

install cmp:

clean:
		-rm -f [ea].out f? *.o

opr:
		make pr | opr

pr:
		@pr t[12345].p tstenc.p tstgto.p callc.p cmod.c
