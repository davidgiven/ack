# $Header$
install:
	../../install head_em.s head_em
	../../install tail_mon tail_mon

cmp:
	-../../compare head_em.s head_em
	-../../compare tail_mon tail_mon

clean :

opr :
	make pr | opr

pr:
	pr -h `pwd`/head_em.s head_em.s
	@arch pv tail_mon | pr -h `pwd`/tail_mon
