# $Header$
MACH=pmds4
all:		libmon_o.a head_em.o

install: 	all
		../../install head_em.o head_em
		../../install libmon_o.a tail_mon


cmp:		all
		-../../compare head_em.o head_em
		-../../compare libmon_o.a tail_mon

libmon_o.a:	libmon_s.a
		ASAR=aal ; export ASAR ;\
		march . libmon_o.a

head_em.o:	head_em.s
		$(MACH) -I../../../h -c head_em.s

clean:
		rm -f *.o libmon_o.a

opr :
		make pr | opr

pr:
		@pr `pwd`/head_em.s
		@arch pv libmon_s.a | pr -h `pwd`/libmon_s.a
