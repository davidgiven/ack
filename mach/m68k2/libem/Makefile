LIBDIR=../lib
install:
	rm -f $(LIBDIR)/head_em $(LIBDIR)/tail_em.rt $(LIBDIR)/tail_em.vend
	ln head_em.s $(LIBDIR)/head_em
	ln libem_s.a $(LIBDIR)/tail_em.rt
	ln libem_s.vend.a $(LIBDIR)/tail_em.vend


clean :

opr :
	make pr | opr

pr:
	@ar pv libem_s.a
