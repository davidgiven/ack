CFLAGS=-O

cv:             cv.o
		$(CC) -o cv -n cv.o

pmcv:		pmcv.o
		$(CC) -o pmcv -n pmcv.o

install:	ins_cv ins_pmcv
ins_cv:		cv
		../../install cv
ins_pmcv:	pmcv
		../../install pmcv

cmp:		cmp_cv cmp_pmcv
cmp_cv:		cv
		-../../compare cv
cmp_pmcv:	pmcv
		-../../compare pmcv

opr:
		make pr | opr

pr:
		@pr `pwd`/cv.c `pwd`/pmcv.c

clean:
		-rm -f *.o *.old cv pmcv
