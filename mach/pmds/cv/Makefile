CFLAGS=-O

cv:             cv.o
		cc -o cv -n cv.o

install:	cv
		cp cv ../../../bin

cmp:		cv
		-../compare cv

opr:
		make pr | opr

pr:
		@pr `pwd`/cv.c

clean:
		-rm -f *.o *.old cv
