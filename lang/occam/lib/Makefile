PRIMITIVES=	par_vax.s
PARALLEL=	parco.c par.c co.c
OCRT=		ocrt.c builtin.c channel.c chan_struct.c

COMMON=		$(PRIMITIVES) $(PARALLEL) $(OCRT)

SIZE2=	em2.e
SIZE4=	em4.e

LIB2=	/user0/bot/lib/lib2.a
LIB4=	/user0/bot/lib/lib4.a

ACK2=	vax2
ACK4=	vax4

all:	lib2 lib4

lib2:	$(COMMON) $(SIZE2)
	rm -f *.o $(LIB2)
	$(ACK2) -c.o -L -Dvoid=char -Dptrdiff=long $(COMMON) $(SIZE2)
	ar cq $(LIB2) *.o
	rm -f *.o

lib4:	$(COMMON) $(SIZE4)
	rm -f *.o $(LIB4)
	$(ACK4) -c.o -L -Dvoid=char $(COMMON) $(SIZE4)
	ar cq $(LIB4) *.o
	rm -f *.o
