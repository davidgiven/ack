# $Header$

TBL=tbl

DOC =		draw.mac cover txt1 txt2 txt3 appA appB bib

../int.doc:	int.doc
		mv int.doc ..

int.doc:	$(DOC)
		$(TBL) $(DOC) > $@

FLS =		README Makefile $(DOC)

.distr:		Makefile
		echo $(FLS) | tr ' ' '\012' >.distr

distr:	.distr

clean:
		rm -f int.doc

