CFLAGS=-I../../../h -O
LDFLAGS =

cv:             cv.o
		$(CC) -o cv $(LDFLAGS) cv.o

install:	ins_cv
ins_cv:		cv
		../../install cv

cmp:		cmp_cv
cmp_cv:		cv
		-../../compare cv

opr:
		make pr | opr

pr:
		@pr `pwd`/cv.c

clean:
		-rm -f *.o *.old cv
