# $Header$

d=../../..
h=$d/h
l=$d/lib
INSTALL=$l/em_bem

CFLAGS = -I$h -O

FILES= bem.o y.tab.o symbols.o initialize.o compile.o \
	parsepar.o yywrap.o gencode.o util.o graph.o \
	eval.o func.o split.o

CFILES= bem.c y.tab.c symbols.c initialize.c compile.c \
	parsepar.c yywrap.c gencode.c util.c graph.c \
	eval.c func.c split.c

SRC=    bem.h symbols.h graph.h y.tab.h \
	bem.c basic.yacc symbols.c initialize.c compile.c \
	parsepar.c yywrap.c gencode.c util.c graph.c \
	eval.c func.c split.c

first :		bem

cmp :		bem
		cmp bem $(INSTALL)

install:	bem
		cp bem $(INSTALL)

clean:
		rm -f *.[osmk] *.old bem

opr:
		make pr ^ opr
pr:
		@pr $(SRC)

distr:		y.tab.c y.tab.h

bem:		$(FILES)
		$(CC) -o bem $(FILES)

y.tab.o : 	y.tab.c basic.lex
		$(CC) $(CFLAGS) -c y.tab.c

y.tab.h y.tab.c : basic.yacc 
		yacc -d basic.yacc

$(FILES)	: bem.h symbols.h graph.h y.tab.h

lint:		$(CFILES)
		lint -phac -I$h $(CFILES)
