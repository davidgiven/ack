# $Header$

EM =		../../..
IP_SPEC =	$(EM)/etc/ip_spec.t
CFLAGS =	-I$(EM)

SRC =		mkiswitch.c	mkswitch.c
OBJ =		mkiswitch.o	mkswitch.o

mkiswitch:	mkiswitch.o
	$(CC) -o mkiswitch mkiswitch.o $(EM)/lib/em_data.a

mkswitch:	mkswitch.o
	$(CC) -o mkswitch mkswitch.o

DoCases:	mkiswitch $(IP_SPEC)
	mkiswitch Do $(IP_SPEC) DoCases
	wc DoCases

PrCases:	mkswitch $(IP_SPEC)
	mkswitch Pr $(IP_SPEC) PrCases
	wc PrCases

lint:
	lint $(SRC)

distr:		.distr

.distr:		Makefile
	echo READ_ME Makefile $(SRC) | tr ' ' '\012' >.distr

.SUFFIXES:	.o
.c.o:
	$(CC) $(CFLAGS) -c $<

clean:						# NOT the cases files !
	rm -f a.out core $(OBJ) mkswitch mkiswitch

bare:		clean
	rm -f DoCases PrCases

all:	mkswitch

install:
	echo "Nothing to install"

cmp:
	echo "Nothing to compare"

pr:
	@pr READ_ME Makefile $(SRC)

opr:
	make pr | opr
