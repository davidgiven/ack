# $Header$
install:	cp

cp:             all
		../../install tail_mon
		rm -f head_em tail_mon

cmp:            all
		-../../compare tail_mon
		rm -f head_em tail_mon

all:		tail_mon

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
