GFILES=		occam.g
PRIMARY=	occam.o Lpars.o keytab.o lex.yy.o code.o em.o
SECUNDARY=	symtab.o expr.o builtin.o
TERTIARY=	report.o
LLOPT=
LIBRARY=	-lln libemk.a libsystem.a

all:
		make dummy
		make oc

dummy:		$(GFILES)
		LLgen $(LLOPT) $(GFILES)
		touch dummy

oc:		$(PRIMARY) $(SECUNDARY) $(TERTIARY)
		$(CC) -o oc $(PRIMARY) $(SECUNDARY) $(TERTIARY) $(LIBRARY)

lex.yy.c:	lex.l
		lex lex.l

$(PRIMARY):			Lpars.h
occam.o keytab.o:		token.h
occam.o $(SECUNDARY):		symtab.h expr.h
$(PRIMARY) $(SECUNDARY):	sizes.h
occam.o code.o:			code.h
code.o em.o:			em.h
