EMHOME =	../../..
LIBOBJ =	$(EMHOME)/modules/lib/libobject.a
INCLUDE =	$(EMHOME)/h
CFLAGS =	-I. -I$(INCLUDE) -O
TARGETS =	cv Xcv

all:		$(TARGETS)

install:	all
		../../install cv
		../../install Xcv

cmp:		all
		../../compare cv
		../../compare Xcv

cv:	cv.o
		$(CC) $(LDFLAGS) -o cv cv.o $(LIBOBJ)

Xcv:	Xcv.o
		$(CC) $(LDFLAGS) -o Xcv Xcv.o $(LIBOBJ)

clean:
		rm -f $(TARGETS) *.o nohup.out Out

pr:
		@pr Makefile cv.c Xcv.c

opr:
		make pr | opr
