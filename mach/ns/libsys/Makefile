# $Header$
MACH=ns
all:		tail_mon head_em.o

install: 	all
		../../install head_em.o head_em
		../../install tail_mon

cmp:		all
		-../../compare head_em.o head_em
		-../../compare tail_mon

tail_mon:	tail_mon.a
		ASAR=aal ; export ASAR ;\
		march . tail_mon

head_em.o:	head_em.s
		$(MACH) -I../../../h -c head_em.s

clean:
		rm -f *.o

opr :
		make pr | opr

pr:
		@pr -h `pwd`/head_em.s head_em.s
		@ar pv tail_mon.a | pr -h `pwd`/tail_mon.a
