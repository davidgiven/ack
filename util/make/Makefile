# Makefile for make!
EMHOME=../..

OBJS	=	check.o input.o macro.o main.o \
		make.o reader.o rules.o
# unix not defined under Xenix ???
CFLAGS = -Dunix	

all:	make

install: make
	cp make $(EMHOME)/bin/make

cmp:	make
	cmp make $(EMHOME)/bin/make

clean:
	rm -f *.o make

pr:
	@pr README Makefile h.h main.c check.c input.c macro.c make.c reader.c rules.c

opr:
	make pr ^ opr

make:	$(OBJS)
	$(CC) -o make $(OBJS)

$(OBJS):	h.h
