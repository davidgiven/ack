# $Header$
MACH=i86
install: 	all
		../../install head_em.o head_em
		../../install libem_o.a tail_em
		../../install end.o end_em

cmp:		all
		-../../compare head_em.o head_em
		-../../compare libem_o.a tail_em
		-../../compare end.o end_em

all:            head_em.o libem_o.a end.o

end.o:		end.s
		$(MACH) -I../../../h -c end.s

head_em.o:      head_em.s
		$(MACH) -I../../../h -c head_em.s

libem_o.a:	libem_s.a
		ASAR=aal ; export ASAR ;\
		march . libem_o.a

clean:
		rm -f *.o
opr :
		make pr | opr

pr:
		@pr `pwd`/head_em.s
		@arch pv libem_s.a | pr -h `pwd`/libem_s.a
		@pr `pwd`/end.s
