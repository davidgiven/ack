
install:	cp

cp:
		ln tail_em.s.a head_em ; ../../install head_em ; rm head_em
		ln tail_em.s.a tail_em ; ../../install tail_em ; rm tail_em

cmp:
		-ln tail_em.s.a head_em ; ../../compare head_em ; rm head_em
		-ln tail_em.s.a tail_em ; ../../compare tail_em ; rm tail_em
