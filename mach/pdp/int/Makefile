INTS=em_t--- em_---- em_tf-- em_t-c- em_t--p
b=../../../lib/pdp_int

all:	$(INTS) eminform em

em:	em.c
	cc -o em -I../../../h em.c

eminform:	eminform.s
	as eminform.s;ld -i -o eminform a.out -lc

em_t---:	t+ f- c- p- em_int.s
	as t+ f- c- p- em_int.s;ld -i -o em_t--- a.out -lc

em_----:	t- f- c- p- em_int.s
	as t- f- c- p- em_int.s;ld -i -o em_---- a.out -lc

em_tf--:	t+ f+ c- p- em_int.s
	as t+ f+ c- p- em_int.s;ld -i -o em_tf-- a.out -lc

em_t-c-:	t+ f- c+ p- em_int.s
	as t+ f- c+ p- em_int.s;ld -i -o em_t-c- a.out -lc

em_t--p:	t+ f- c- p+ em_int.s
	as t+ f- c- p+ em_int.s;ld -i -o em_t--p a.out -lc

install:	all
		-mkdir $b
		cp em_???? $b
		cp em eminform ../../../bin

cmp: 		all
		cmp em_t--- $b/em_t---
		cmp em_---- $b/em_----
		cmp em_tf-- $b/em_tf--
		cmp em_t-c- $b/em_t-c-
		cmp em_t--p $b/em_t--p
		cmp em ../../../bin/em
		cmp eminform ../../../bin/eminform

clean:
		-rm -f *.o *.old a.out em eminform $(INTS)

opr:
		make pr | opr

pr:
		@pr em.c em_int.s eminform.s
