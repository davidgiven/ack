# $Header$

NOFL =

all:            testC testI

testI:
#		int $(NOFL) t1.p; em
		int $(NOFL) t2.p; em
		int $(NOFL) t3.p; em e.out f1 f2 f3 f4 f5 f6
		int $(NOFL) t4.p; em
		int $(NOFL) t5.p; em
		int $(NOFL) tstenc.p; em
		int $(NOFL) tstgto.p; em
		int $(NOFL) -.p callc.p cmod.c ; em
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
