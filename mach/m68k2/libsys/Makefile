# $Header$
MACH=m68k2
all:		tail_mon
install: 	all
		../../install tail_mon

cmp:		all
		-../../compare tail_mon

tail_mon:
		ASAR=aal ; export ASAR ;\
		march . tail_mon

clean:
		rm -f *.o

opr :
		make pr | opr

pr:
		@ar pv tail_mon.a
