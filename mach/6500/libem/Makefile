install:
	../../install head.s head_em
	../../install e.a tail_em
	../../install libem_s.vend.a tail_em.vend

cmp:
	-../../compare head.s head_em
	-../../compare e.a tail_em
	-../../compare libem_s.vend.a tail_em.vend

clean :

opr :
	make pr | opr

pr:
	@pr head_em.s
	@arch pv e.a | pr -h `pwd`/e.a
	@arch pv libem_s.vend.a | pr -h `pwd`/libem_s.vend.a
