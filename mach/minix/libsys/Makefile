# $Header$
MACH=minix

all:            libsys_o.a end.o head_em.o

install: 	all
		../../install libsys_o.a tail_mon
		../../install head_em.o head_em
		../../install end.o end_em

cmp:		all
		-../../compare libsys_o.a tail_mon
		-../../compare head_em.o head_em
		-../../compare end.o end_em


end.o:		end.s
		$(MACH) -I../../../h -O -c end.s

head_em.o:	head_em.s
		$(MACH) -I../../../h -O -c head_em.s

libsys.a:	libsys_s.a
		ASAR=aal ; export ASAR ;\
		march . libsys.a

libsys_o.a:	libsys.a ../../../lib/i86/tail_em
		mkdir X; cd X; aal x ../../../../lib/i86/tail_em; aal x ../libsys.a; aal rv ../libsys_o.a *.o ; cd ..
		rm -rf X

clean:
		rm -f *.o libsys_o.a libsys.a

opr :
		make pr | opr

pr:
		@pr `pwd`/head_em.s
		@arch pv libsys.a | pr -h `pwd`/libsys.a
		@pr `pwd`/end.s
