# $Header$

head:
	echo This Makefile needs arguments

distr:
	rm `head -1 LIST`; arch cr `head -1 LIST` `tail +2 LIST`

clean:
	rm -f *.old

opr:
	make pr | opr

pr:
	@pr Makefile *.[ec]
