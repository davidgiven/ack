.SILENT:
CEM=pdp
head:   gen
diffs:
	echo No diffs in ctprof
egen:   tp.e
	echo comparing tp.e
	-if test -f tp.e.g ; then diff tp.cem.r tp.e.g ; else echo creating tp.e.g ; cp tp.cem.r tp.e.g ; fi
	rm -f tp.e
tp.e:   tp.c
	$(CEM) -p -c.e tp.c
procentry.m: procentry.c
	$(CEM) -c.m procentry.c
tp.cem: tp.c procentry.m
	echo $(CEM) tp.c procentry.m
	$(CEM) -p -o tp.cem -O tp.c procentry.m
	rm -f procentry.[kosm] tp.[kmos]

pr:
	@pr `pwd`/*.c tp.cem.g

opr:
	make pr | opr

clean:
	-rm -f *.[kosme] *.old em_last em_runinf

transI transM cmpI cmpM:
