# $Header$
all:            head_em.o libmon_o.a

install:	cp

cp:             all
		../../install head_em.o head_em
		../../install libmon_o.a tail_mon

cmp:            all
		-../../compare head_em.o head_em
		-../../compare libmon_o.a tail_mon


head_em.o:      head_em.s
		vax4 -I../../../h -c head_em.s

libmon_o.a:	libmon_s.a
		ASAR=ar ; export ASAR ;\
		RANLIB=ranlib ; export RANLIB ;\
		march . libmon_o.a

clean:
		rm -f *.o libmon_o.a

opr:
		make pr | opr

pr:
		@pr `pwd`/Makefile `pwd`/head_em.s
		@arch pv libmon_s.a | pr -h `pwd`/libmon_s.a
