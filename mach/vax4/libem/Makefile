TAILSRC=tail_em.a
l=../lib

install:	head_em tail_em
		cp head_em $l/head_em
		cp tail_em $l/tail_em
		-rm -f head_em tail_em

cmp:		head_em tail_em
		-cmp head_em $l/head_em
		-cmp tail_em $l/tail_em

head_em:        head_em.s system.h
		vax4 -c -I../../../h head_em.s
		mv head_em.o head_em

tail_em:        $(TAILSRC) system.h
		arch x $(TAILSRC) ;\
		ALL= ; \
		for i in `arch t $(TAILSRC)` ; do \
			BN=`basename $$i .s`.o ; \
			vax4 -I../../../h -c $$i ; \
			RM="$$RM $$i" ; \
			ALL="$$ALL $$BN" ; \
		done ; \
		ar r tail_em $$ALL ; rm -f $$RM $$ALL

clean:
		-rm -f `arch t $(TAILSRC)` *.old *.o

opr:
		make pr | opr

pr:
		@(pr head_em.s ; arch pv $(TAILSRC) | pr -h "vax4/libem" )
