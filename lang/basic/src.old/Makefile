CFLAGS = -c

FILES= bem.o y.tab.o symbols.o initialize.o compile.o \
	parseparams.o yywrap.o gencode.o util.o graph.o \
	eval.o func.o split.o

../bem: $(FILES)
	cc -o ../bem $(FILES)

y.tab.o : y.tab.c lex.c
	cc $(CFLAGS) y.tab.c

y.tab.c : basic.yacc 
	yacc -d basic.yacc
