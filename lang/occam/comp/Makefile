EMHOME =	../../..
MODLIB =	$(EMHOME)/modules/lib
INCL =		-I$(EMHOME)/modules/h -I$(EMHOME)/h
GFILES=		occam.g
PRIMARY=	occam.o Lpars.o keytab.o lex.yy.o code.o em.o
SECUNDARY=	symtab.o expr.o builtin.o
TERTIARY=	report.o
LLOPT=
LEXLIB=-ll
LIBRARY=	$(MODLIB)/libem_mes.a $(MODLIB)/libemk.a \
		$(MODLIB)/liballoc.a $(MODLIB)/libprint.a \
		$(MODLIB)/libstring.a \
		$(MODLIB)/libsystem.a $(LEXLIB)
CFLAGS =	-Dvoid=int -O $(INCL)
LDFLAGS =
# void = int, because some compilers don't understand void
HSRC =		code.h em.h expr.h sizes.h symtab.h token.h
CSRC =		builtin.c code.c em.c expr.c keytab.c report.c symtab.c

all:		dummy
		make oc

dummy:		$(GFILES)
		LLgen $(LLOPT) $(GFILES)
		touch dummy

oc:		$(PRIMARY) $(SECUNDARY) $(TERTIARY)
		$(CC) $(LDFLAGS) -o oc $(PRIMARY) $(SECUNDARY) $(TERTIARY) $(LIBRARY)

lex.yy.c:	lex.l
		lex lex.l

install:	all
		rm -f $(EMHOME)/lib/em_occam
		cp oc $(EMHOME)/lib/em_occam

cmp:		all
		-cmp oc $(EMHOME)/lib/em_occam

pr:
		@pr Makefile $(HSRC) occam.g lex.l $(CSRC)

opr:
		make pr | opr

clean:
		rm -f lex.yy.c occam.c *.o oc dummy Lpars.c Lpars.h

$(PRIMARY):			Lpars.h
occam.o keytab.o:		token.h
occam.o $(SECUNDARY):		symtab.h expr.h
$(PRIMARY) $(SECUNDARY):	sizes.h
occam.o code.o:			code.h
code.o em.o:			em.h
