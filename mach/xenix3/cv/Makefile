EMHOME =	../../..
LIBOBJ =	$(EMHOME)/modules/lib/libobject.a
INCLUDE =	$(EMHOME)/h
CFLAGS =	-I. -I$(INCLUDE) -O
TARGETS =	cv chstack

all:		$(TARGETS)

install:	all
		../../install cv
		../../install chstack

cmp:		all
		-../../compare cv
		-../../compare chstack

cv:	cv.o
		$(CC) $(LDFLAGS) -o cv cv.o $(LIBOBJ)

chstack:	chstack.o
		$(CC) $(LDFLAGS) -o chstack chstack.o

clean:
		rm -f $(TARGETS) *.o nohup.out Out

pr:
		@pr Makefile cv.c chstack.c

opr:
		make pr | opr
