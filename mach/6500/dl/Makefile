CFLAGS=-O

dl:             dl.o
		cc -o dl -n dl.o

install:	dl
		../../install dl

cmp:		dl
		-../../compare dl

opr:
		make pr | opr

pr:
		@pr `pwd`/dl.c

clean:
		-rm -f *.o *.old dl
