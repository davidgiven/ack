# $Id$

FLOATS=
# for machines without floating point:
FLOATS=-DNOFLOAT
# for machines with software floating point library:
FLOATS=-fp
PC = apc
INTPC = em44
INT = int

all:            testC testI

testI:
		$(INTPC) $(FLOATS) t1.p; $(INT)
		$(INTPC) $(FLOATS) t2.p; $(INT)
		$(INTPC) $(FLOATS) t3.p; $(INT) e.out f1 f2 f3 f4 f5 f6
		$(INTPC) $(FLOATS) t4.p; $(INT)
		$(INTPC) $(FLOATS) t5.p; $(INT)
		$(INTPC) $(FLOATS) tstenc.p; $(INT)
		$(INTPC) $(FLOATS) tstgto.p; $(INT)
		$(INTPC) $(FLOATS) -.p callc.p cmod.c ; $(INT)
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
