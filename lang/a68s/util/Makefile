EM=../../..
h=$EM/h

APC=apc
ACC=acc

all:		tailor xref checkseq reseq indent68

install:	all
		cp indent68 $(EM)/bin/indent68

cmp:		all

tailor:		tailor.p
		$(APC) -o tailor tailor.p

indent68:	indent.p
		$(APC) -o indent68 indent.p

xref:		xref.c
		cc -o xref xref.c

checkseq:	checkseq.p
		$(APC) -o checkseq checkseq.p

reseq:		reseq.p
		$(APC) -o reseq reseq.p

clean:	
		-rm -f *.o indent68

pr:	
		@pr tailor.p xref.c checkseq.p reseq.p indent.p

opr:	
		make pr ^ opr
