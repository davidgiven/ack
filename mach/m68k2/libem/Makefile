install:
	../../install head_em.s head_em
	../../install libem_s.a tail_em.rt
	../../install libem_s.vend.a tail_em.vend

cmp:
	../../compare head_em.s head_em
	../../compare libem_s.a tail_em.rt
	../../compare libem_s.vend.a tail_em.vend

clean :

opr :
	make pr | opr

pr:
	@pr head_em.s
	@arch pv libem_s.a | pr -h `pwd`/libem_s.a
	@arch pv libem_s.vend.a | pr -h `pwd`/libem_s.vend.a
