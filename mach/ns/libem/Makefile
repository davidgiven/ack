# $Header$
MACH=ns

all:            libem_o.a end.o

install: 	all
		../../install libem_o.a tail_em
		../../install end.o end_em

cmp:		all
		-../../compare libem_o.a tail_em
		-../../compare end.o end_em

end.o:		end.s
		$(MACH) -I../../../h -c end.s

libem_o.a:	libem_s.a
		ASAR=aal ; export ASAR ;\
		march . libem_o.a

clean:
		rm -f *.o
opr :
		make pr | opr

pr:
		@arch pv libem_s.a | pr -h `pwd`/libem_s.a
		@pr `pwd`/end.s
