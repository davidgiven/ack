.SILENT:
CEM=acc
head:
	echo use run

diffs:	$P.cc.r $P.cem.r
	echo two compiler diff
	-diff $P.*.r

diffs3:  $P.pcc.r $P.cc.r $P.cem.r
	echo three compiler diff
	-diff3 $P.*.r | tee diffs
egen:   $P.e
	echo comparing $P.e
	-if test -f $P.e.g ; then diff -h $P.e $P.e.g ; else echo creating $P.e.g ; cp $P.e $P.e.g ; fi
	rm -f $P.e
$P.e:   $P.c
	$(CEM) -c.e $P.c
$P.pcc.r: $P.pcc
	echo running $P.pcc
	-$P.pcc >$P.pcc.r
	rm -f $P.pcc
$P.cc.r:        $P.cc
	echo running $P.cc
	-$P.cc >$P.cc.r
	rm -f $P.cc
$P.cem.r: $P.cem
	echo running $P.cem
	-$P.cem >$P.cem.r
	rm -f $P.cem
$P.pcc: /tmp
	echo pcc $P.c
	pcc -o $P.pcc $P.c
$P.cc: /tmp
	echo cc $P.c
	cc -o $P.cc $P.c
$P.cem: /tmp
	echo $(CEM) $P.c
	$(CEM) -o $P.cem  $P.c
gen:    $P.cem.r
	echo comparing $P
	-if test -f $P.cem.g ; then diff -h $P.cem.r $P.cem.g ; else echo creating $P.cem.g ; cp $P.cem.r $P.cem.g ; fi

install cmp:

pr:
	@pr `pwd`/$P.c `pwd`/$P.cem.g

opr:
	make pr | opr

clean:
	-rm -f $P.[kmsoe] $P.*.r core a.out *.old em_last em_runinf e.out
