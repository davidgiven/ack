l=../lib

install:	cp

cp:             all
		cp head_em $l/head_em
		cp tail_em $l/tail_em
		rm -f head_em tail_em

cmp:            all
		cmp head_em $l/head_em
		cmp tail_em $l/tail_em
		rm -f head_em tail_em

all:            head_em tail_em

head_em:        head_em.s
		pdp -c head_em.s ; mv head_em.o head_em

tail_em:        tail_em.s.a
		mkdir tmp; \
		cd tmp; \
		ar x ../tail_em.s.a; \
		for i in *.s; \
		do \
		  pdp -c $$i; \
		done; \
		ar cr tail.a `lorder *.o | tsort`; \
		cp tail.a ../tail_em; \
		cd ..; \
		rm -r tmp

clean:
		-rm -f `ar t tail_em.s.a` ; rm -rf temp ; rm -f *.o
opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile `pwd`/head_em.s
		@ar pv tail_em.s.a | pr -h `pwd`/tail_em.s.a
