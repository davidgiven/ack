EMHOME=../../..
SRC =	em.s atof.s fpp.s mli4.s dvu4.s dvi4.s eb.s
OBJ =	em.o atof.o fpp.o mli4.o dvu4.o dvi4.o eb.o
DLSRC =	dl.c dlbin.c
LIBOBJ = $(EMHOME)/modules/lib/libobject.a

CFLAGS =	-O -I$(EMHOME)/h

.s.o:
	z80 -c -I$(EMHOME)/h $*.s

all:	int.hex dl dlbin cv

install:	all
	@echo "nothing is installed"

cmp:	all
	@echo "nothing is compared"

clean:
	rm -f *.o a.out dl dlbin *.hex cv

pr:
	@pr Makefile $(SRC) $(DLSRC) cv.c

opr:
	make pr | opr

a.out:	$(OBJ)
	$(EMHOME)/lib/em_led -b0:0x100 -a0:2 -a1:2 -a2:2 -a3:2 $(OBJ)

int.hex:	a.out dlbin
	./dlbin a.out > int.hex
	./dosort int.hex

dl:	dl.o
	$(CC) -n -o dl dl.o

dlbin:	dlbin.o
	$(CC) -n -o dlbin dlbin.o $(LIBOBJ)

cv:	cv.o
	$(CC) -n -o cv cv.o
