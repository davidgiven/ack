# $Header$
install:
	../../install tail_em.a tail_em
	../../install tail.s end_em

cmp:
	-../../compare tail_em.a tail_em
	-../../compare tail.s end_em

clean :

opr :
	make pr | opr

pr:
	@arch pv tail_em.a | pr -h `pwd`/tail_em.a
	@pr `pwd`/tail.s
