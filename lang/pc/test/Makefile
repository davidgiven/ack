all:            testC testI

testI:
#		int t1.p; em
		int t2.p; em
		int t3.p; em e.out f1 f2 f3 f4 f5 f6
		int t4.p; em
		int t5.p; em
		int tstenc.p; em
		rm -f e.out f?

testC:
		apc t1.p; a.out
		apc t2.p; a.out
		apc t3.p; a.out f1 f2 f3 f4 f5 f6
		apc t4.p; a.out
		apc t5.p; a.out
		apc tstenc.p; a.out
		rm -f a.out f?

install cmp:

clean:
		-rm -f [ea].out f?

opr:
		make pr | opr

pr:
		@pr t[12345].p tstenc.p
