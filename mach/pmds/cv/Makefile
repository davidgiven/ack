CFLAGS=-O

cv:		cv.o
		$(CC) -o cv -n cv.o

install:	cv
		../../install cv

cmp:		cv
		-../../compare cv

opr:
		make pr | opr

pr:
		@pr `pwd`/cv.c

clean:
		-rm -f *.o *.old cv
