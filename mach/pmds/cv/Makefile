EMHOME=../../..

LDFLAGS =
CFLAGS=-O -I$(EMHOME)/h

cv:		cv.o
		$(CC) -o cv $(LDFLAGS) cv.o $(EMHOME)/modules/lib/libobject.a

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
