install:
	../../install head.s head_em
	../../install e.a tail_em
	../../install libem_s.vend.a tail_em.vend

cmp:
	-../../compare head.s head_em
	-../../compare e.a tail_em
	-../../compare libem_s.vend.a tail_em.vend

distr:
		arch cr `cat LIST`

clean:
		rm -f *.o
opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile `pwd`/head_em.s
		pr -l33 `tail +1 LIST|sort`
clean :

opr :
	make pr | opr

pr:
	@pr head_em.s
	@arch pv e.a | pr -h `pwd`/e.a
	@arch pv libem_s.vend.a | pr -h `pwd`/libem_s.vend.a
