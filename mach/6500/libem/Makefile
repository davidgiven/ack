install:	tail_em.s.a tail_em.ve.s.a
		../../install head_em.s head_em
		../../install tail_em.s.a tail_em
		../../install tail_em.ve.s.a tail_em.vend

cmp:		tail_em.s.a tail_em.ve.s.a
		-../../compare head_em.s head_em
		-../../compare tail_em.s.a tail_em
		-../../compare tail_em.ve.s.a tail_em.vend

distr:		tail_em.ve.s.a

tail_em.s.a:
		arch cr `cat LIST`

tail_em.ve.s.a:
		arch cr tail_em.ve.s.a end.s

opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile `pwd`/head_em.s
		@pr -l33 `tail +1 LIST|sort` `pwd`/end.s
