# $Header$
MACH=i86
install: 	all
		../../install head_em.o head_em
		../../install tail_em
		../../install end.o end_em

cmp:		all
		-../../compare head_em.o head_em
		-../../compare tail_em
		-../../compare end.o end_em

all:            head_em.o tail_em end.o

end.o:		end.s
		$(MACH) -I../../../h -c end.s

head_em.o:      head_em.s
		$(MACH) -I../../../h -c head_em.s

tail_em:	tail_em.a
		ASAR=aal ; export ASAR ;\
		march . tail_em

clean:
		rm -f *.o
opr :
		make pr | opr

pr:
		@pr `pwd`/head_em.s
		@arch pv tail_em.a | pr -h `pwd`/tail_em.a
		@pr `pwd`/end.s
