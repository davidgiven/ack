# $Header$

DOC =		draw.mac cover txt1 txt2 txt3 appA appB bib

FLS =		README proto.make Makefile $(DOC)

.distr:		Makefile
		echo $(FLS) | tr ' ' '\012' >.distr

distr:	.distr
