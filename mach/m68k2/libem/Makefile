install:
	ln head_em.s head_em ; ../../install head_em ; rm head_em
	ln libem_s.a tail_em.rt ; ../../install tail_em.rt ; rm tail_em.rt
	ln libem_s.vend.a tail_em.vend ; ../../install tail_em.vend ;\
		rm tail_em.vend

clean :

opr :
	make pr | opr

pr:
	@ar pv libem_s.a
