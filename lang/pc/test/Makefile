# $Header$

all:            testC testI

testI:
#		int t1.p; em
		int t2.p; em
		int t3.p; em e.out f1 f2 f3 f4 f5 f6
		int t4.p; em
		int t5.p; em
		int tstenc.p; em
		int tstgto.p; em
		int -.p callc.p cmod.c ; em
		rm -f e.out f? *.k

testC:
		apc t1.p; a.out
		apc t2.p; a.out
		apc t3.p; a.out f1 f2 f3 f4 f5 f6
		apc t4.p; a.out
		apc t5.p; a.out
		apc tstenc.p; a.out
		apc tstgto.p; a.out
		apc -.p callc.p cmod.c ; em
		rm -f a.out f? *.[os]

install cmp:

clean:
		-rm -f [ea].out f?

opr:
		make pr | opr

pr:
		@pr t[12345].p tstenc.p
