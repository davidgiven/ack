# $Header$
MACH=z80
all:		libem_o.a end.o

install: 	all
		../../install end.o end_em
		../../install libem_o.a tail_em


cmp:		all
		-../../compare end.o end_em
		-../../compare libem_o.a tail_em

libem_o.a:	libem_s.a
		ASAR=aal ; export ASAR ;\
		march . libem_o.a

end.o:		end.s
		$(MACH) -I../../../h -c end.s

clean:
		rm -f *.o libem_o.a

opr :
		make pr | opr

pr:
		@pr `pwd`/end.s
		@arch pv libem_s.a | pr -h `pwd`/libem_s.a
