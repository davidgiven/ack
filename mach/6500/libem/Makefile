all:

install:
		../../install head_em.s head_em
		../../install libem_s.a tail_em
		../../install end.s end_em

cmp:
		-../../compare head_em.s head_em
		-../../compare libem_s.a tail_em
		-../../compare end.s end_em

opr:
		make pr | opr

pr:
		@pr `pwd`/head_em.s
		@arch pv libem_s.a | pr -h `pwd`/libem_s.a
		@pr `pwd`/end.s

clean:
