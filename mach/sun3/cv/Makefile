EMHOME =	../../..
LIBOBJ =	$(EMHOME)/modules/lib/libobject.a
INCLUDE =	$(EMHOME)/h
CFLAGS =	-I. -I$(INCLUDE) -O -DMACH=2
TARGETS =	cv

all:		$(TARGETS)

install:	all
		../../install cv

cmp:		all
		../../compare cv

cv:	cv.o
		$(CC) $(LDFLAGS) -o cv cv.o $(LIBOBJ)

Xcv:	Xcv.o
		$(CC) $(LDFLAGS) -o Xcv Xcv.o $(LIBOBJ)

clean:
		rm -f $(TARGETS) *.o nohup.out Out

pr:
		@pr Makefile cv.c

opr:
		make pr | opr
