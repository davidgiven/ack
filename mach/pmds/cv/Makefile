CFLAGS=-O

pmcv:		pmcv.o
		$(CC) -o pmcv -n pmcv.o

pmcv.c:
		case `ack_sys` in \
		vax*)	cp vax_cv.c pmcv.c ;; \
		pdp*)	cp pdp_cv.c pmcv.c ;; \
		*)	echo "A conversion program should be present in `pwd`/pmcv.c" ;\
			exit 9 ;; \
		esac

install:	ins_pmcv
ins_pmcv:	pmcv
		../../install pmcv

cmp:		cmp_pmcv
cmp_pmcv:	pmcv
		-../../compare pmcv

opr:
		make pr | opr

pr:
		@pr `pwd`/pmcv.c

clean:
		-rm -f *.o *.old pmcv
