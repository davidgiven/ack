# $Header$
MACH=i80
all:		libsys_o.a head_em.o

install: 	all
		../../install head_em.o head_em
		../../install libsys_o.a tail_sys


cmp:		all
		-../../compare head_em.o head_em
		-../../compare libsys_o.a tail_sys

libsys_o.a:	libsys_s.a
		ASAR=aal ; export ASAR ;\
		march . libsys_o.a

head_em.o:	head_em.s
		$(MACH) -I../../../h -c head_em.s

clean:
		rm -f *.o libsys_o.a

opr :
		make pr | opr

pr:
		@pr `pwd`/head_em.s
		@arch pv libsys_s.a | pr -h `pwd`/libsys_s.a
