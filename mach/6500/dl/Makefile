EMHOME=../../..
CFLAGS=-O -I$(EMHOME)/h

dl:             dl.o
		cc -o dl -n dl.o $(EMHOME)/modules/lib/libobject.a

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
