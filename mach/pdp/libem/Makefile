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

distr:
		arch cr `head -1 LIST` `tail +2 LIST`

all:            head_em tail_em

head_em:        head_em.s
		pdp -c head_em.s ; mv head_em.o head_em

tail_em:
		march . tail_em

clean:
		rm -f *.o
opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile `pwd`/head_em.s
		pr -l33 `tail +1 LIST|sort`
