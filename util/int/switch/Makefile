# $Header$

IP_SPEC =	/usr/em/etc/ip_spec.t

SRC =	 	mkswitch.c
OBJ =		mkswitch.o

mkswitch:	$(OBJ)
	$(CC) -o mkswitch $(OBJ)

DoCases:	mkswitch $(IP_SPEC)
	mkswitch Do $(IP_SPEC) DoCases
	wc DoCases

PrCases:	mkswitch $(IP_SPEC)
	mkswitch Pr $(IP_SPEC) PrCases
	wc PrCases

lint:
	lint $(SRC)

.distr:		Makefile
	echo READ_ME Makefile $(SRC) | tr ' ' '\012' >.distr

.SUFFIXES:	.o
.c.o:
	$(CC) $(CFLAGS) -c $<

clean:						# NOT the cases files !
	/bin/rm -f a.out core $(OBJ) mkswitch

bare:		clean
	/bin/rm -f DoCases PrCases

