# $Header$

EMHOME=../../..
h=$(EMHOME)/h
m=$(EMHOME)/modules/h
LIBDIR= $(EMHOME)/modules/lib
LIBDIR2= $(EMHOME)/lib
CFLAGS = -I$h -I$m

FILES= bem.o symbols.o initialize.o compile.o \
	parsepar.o gencode.o util.o graph.o \
	eval.o func.o basic.o Lpars.o

CSRCFILES= bem.c symbols.c initialize.c compile.c \
	parsepar.c gencode.c util.c graph.c \
	eval.c func.c
CGENFILES= basic.c Lpars.c
CFILES=$(CSRCFILES) $(CGENFILES)

LIBFILES= $(LIBDIR)/libem_mes.a $(LIBDIR)/libeme.a \
	  $(LIBDIR2)/em_data.a $(LIBDIR)/libprint.a \
	  $(LIBDIR)/liballoc.a \
	  $(LIBDIR)/libsystem.a $(LIBDIR)/libstring.a

LINTLIBFILES= $(LIBDIR)/llib-lem_mes.a $(LIBDIR)/llib-leme.a \
	  $(LIBDIR)/llib-lprint.a \
	  $(LIBDIR)/llib-lalloc.a \
	  $(LIBDIR)/llib-lsystem.a $(LIBDIR)/llib-lstring.a

all:		dummy bem

dummy:		basic.g
		LLgen basic.g
		touch dummy

install:	all
		cp bem $(EMHOME)/lib/em_bem

cmp:		all
		cmp bem $(EMHOME)/lib/em_bem

pr:
		@pr Makefile maketokentab bem.h symbols.h graph.h basic.g basic.lex $(CSRCFILES)

opr:
		make pr | opr

bem:		$(FILES) $(LIBFILES)
		$(CC) -o bem $(FILES) $(LIBFILES)

basic.o : 	basic.c basic.lex Lpars.h llmess.c tokentab.h
		$(CC) $(CFLAGS) -c basic.c

$(FILES): 	bem.h symbols.h graph.h 

tokentab.h:	Lpars.h
		maketokentab

lint:		$(CFILES)
		lint -b $(CFLAGS) $(CFILES) $(LINTLIBFILES)

clean:
		rm -f *.o
		rm -f basic.c Lpars.h Lpars.c dummy tokentab.h bem
