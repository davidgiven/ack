# $Header$
install:	cp

cp:             all
		../../install tail_em
		rm -f tail_em

cmp:            all
		-../../compare tail_em
		rm -f tail_em

all:            tail_em

tail_em:
		ASAR=ar ; export ASAR ;\
		RANLIB=ranlib ; export RANLIB ;\
		march . tail_em

clean:
		rm -f *.o
opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile
		@pr -l33 `tail +1 LIST|sort`
