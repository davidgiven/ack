.SILENT:
CEM=pdp
head:   gen
diffs:
	echo No diffs in ctprof
egen:   tp.e
	echo comparing tp.e
	-if test -f tp.e.g ; then diff tp.cem.r tp.e.g ; else echo creating tp.e.g ; cp tp.cem.r tp.e.g ; fi
	rm -f tp.e
tp.e:   tp.c $(CEM)
	$(CEM) -p -c.e tp.c
tp.cem.r: tp.cem
	echo running tp
	-tp.cem >tp.cem.r
	rm -f tp.cem
procentry.k: procentry.c
	$(CEM) -c.k procentry.c
tp.cem: tp.c procentry.k
	echo $(CEM) tp.c procentry.k
	$(CEM) -p -o tp.cem -O tp.c procentry.k
	rm -f procentry.[kosm] tp.[kmos]
gen:    tp.cem.r
	echo comparing tp
	-if test -f tp.cem.g ; then diff tp.cem.r tp.cem.g ; else echo creating tp.cem.g ; cp tp.cem.r tp.cem.g ; fi

pr:
	@pr `pwd`/*.c tp.cem.g

opr:
	make pr | opr

cleanup:
	-rm -f *.[kosme] *.old

transI transM cmpI cmpM:
