# $Header$
install:	cp

cp:             all
		../../install head_em
		../../install tail_mon
		rm -f head_em tail_mon

cmp:            all
		-../../compare head_em
		-../../compare tail_mon
		rm -f head_em tail_mon

all:            head_em tail_mon

head_em:        head_em.s
		vax4 -I../../../h -c head_em.s ; mv head_em.o head_em

tail_mon:
		ASAR=ar ; export ASAR ;\
		RANLIB=ranlib ; export RANLIB ;\
		march . tail_mon

clean:
		rm -f *.o
opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile `pwd`/head_em.s
		@pr -l33 `tail +1 LIST|sort`
