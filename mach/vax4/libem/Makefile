# $Header$
all:            libem_o.a

install:	cp

cp:             all
		../../install libem_o.a tail_em

cmp:            all
		-../../compare libem_o.a tail_em

libem_o.a:	libem_s.a
		ASAR=ar ; export ASAR ;\
		RANLIB=ranlib ; export RANLIB ;\
		march . libem_o.a

clean:
		rm -f *.o libem_o.a

opr:
		make pr | opr

pr:
		@pr `pwd`/Makefile
		@arch pv libem_s.a | pr -h `pwd`/libem_s.a
