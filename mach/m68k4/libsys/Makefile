# $Header$
install:
	../../install head_em.s head_em
	../../install tail_mon.a tail_mon

compare:
	-../../compare head_em.s head_em
	-../../compare tail_mon.a tail_mon

clean :

opr :
	make pr | opr

pr:
	@pr -h `pwd`/head_em.s head_em.s
	@ar pv tail_mon.a | pr -h `pwd`/tail_mon.a
