# $Header$
install:
	arch t tail_em.a > LIST
	../../install tail_em.a tail_em
	../../install end.s tail_em.vend

cmp:
	-../../compare tail_em.a tail_em
	-../../compare end.s end_em

clean :

opr :
	make pr | opr

pr:
	@arch pv tail_em.a | pr -h `pwd`/tail_em.a
	@pr `pwd`/end.s
