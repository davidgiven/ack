# $Header$
install:
	../../install head_em.s head_em
	../../install tail_em
	../../install end.s end_em

cmp:
	-../../compare head_em.s head_em
	-../../compare tail_em
	-../../compare end.s end_em

clean :

opr :
	make pr | opr

pr:
	@pr `pwd`/head_em.s
	@arch pv tail_em.rt | pr -h `pwd`/tail_em.rt
	@pr `pwd`/end.s
