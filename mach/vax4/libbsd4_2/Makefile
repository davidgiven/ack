# $Header$
install:	cp

cp:             all
		../../install head_em.o head_em
		../../install tail_mon

cmp:            all
		-../../compare head_em.o head_em
		-../../compare tail_mon

all:            head_em tail_mon

head_em:        head_em.s
		vax4 -I../../../h -c head_em.s

tail_mon:	libmon_s.a
		ASAR=aal ; export ASAR ;\
		march . tail_mon

clean:
		rm -f *.o tail_mon
opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile `pwd`/head_em.s
		@pr -l33 `tail +1 LIST|sort`
