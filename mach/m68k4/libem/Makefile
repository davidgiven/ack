# $Header$
MACH=m68k4
ASAR=aal
all:		libem_o.a end.a

install: 	all
		../../install libem_o.a tail_em
		../../install end.a end_em

cmp:		all
		-../../compare libem_o.a tail_em
		-../../compare end.a end_em

end.a:		em_end.s etext.s edata.s end.s
		$(MACH) -I../../../h -c em_end.s
		$(MACH) -I../../../h -c edata.s
		$(MACH) -I../../../h -c etext.s
		$(MACH) -I../../../h -c end.s
		$(ASAR) cr end.a em_end.o etext.o edata.o end.o

libem_o.a:	libem_s.a
		ASAR=$(ASAR) ; export ASAR ;\
		march . libem_o.a

clean:
		rm -f *.o libem_o.a end.a

opr :
		make pr | opr

pr:
		@arch pv libem_s.a | pr -h `pwd`/libem_s.a
		@pr `pwd`/em_end.s `pwd`/edata.s `pwd`/etext.s `pwd`/end.s
