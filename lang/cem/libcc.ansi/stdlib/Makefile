clean:
	rm -f abort.o abs.o atof.o atoi.o atol.o bsearch.o div.o \
		atexit.o exit.o getenv.o labs.o ldiv.o malloc.o mblen.o \
		mbstowcs.o mbtowc.o qsort.o rand.o strtod.o strtol.o \
		system.o wcstombs.o wctomb.o ext_comp.o malloc.c OLIST

malloc/malloc.c:
	-(cd malloc; make)

malloc.c: malloc/malloc.c
	-cp malloc/malloc.c malloc.c

distr:	malloc.c

