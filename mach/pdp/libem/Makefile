all:            head_em.o libem_o.a

install:	cp

cp:             all
		../../install head_em.o head_em
		../../install libem_o.a tail_em

cmp:            all
		-../../compare head_em.o head_em
		-../../compare libem_o.a tail_em


head_em.o:      head_em.s
		pdp -c head_em.s

libem_o.a:	libem_s.a
		ASAR=ar ; export ASAR ; march . libem_o.a

clean:
		rm -f *.o libem_o.a

opr:
		make pr | opr

pr:
		@pr `pwd`/Makefile `pwd`/head_em.s
		@arch pv libem_s.a | pr -h `pwd`/libem_s.a
