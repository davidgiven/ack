
install:	
		../../install head_em.s head_em
		../../install tail_em.s.a tail_em

cmp:
		-../../compare head_em.s head_em
		-../../compare tail_em.s.a tail_em

clean:

opr :
	make pr | opr

pr:
	@pr head_em.s
	@arch pv tail_em.s.a | pr -h `pwd`/tail_em.s.a
