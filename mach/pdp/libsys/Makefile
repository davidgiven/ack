# $Header$
all:		libmon_o.a

install:	cp

cp:             all
		../../install libmon_o.a tail_mon

cmp:            all
		-../../compare libmon_o.a tail_mon

libmon_o.a:	libmon_s.a
		ASAR=aal ; export ASAR ;\
		march . libmon_o.a

clean:
		rm -f *.o libmon_o.a

opr:
		make pr | opr

pr:
		@pr `pwd`/Makefile
		@arch pv libmon_s.a | pr -h `pwd`/libmon_s.a
