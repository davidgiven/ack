CFLAGS=-O

pmcv:		pmcv.o
		$(CC) -o pmcv -n pmcv.o

install:	ins_pmcv
ins_pmcv:	pmcv
		../../install pmcv

cmp:		cmp_pmcv
cmp_pmcv:	pmcv
		-../../compare pmcv

opr:
		make pr | opr

pr:
		@pr `pwd`/pmcv.c

clean:
		-rm -f *.o *.old pmcv
