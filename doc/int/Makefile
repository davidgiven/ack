# $Header$

TBL=tbl

DOC =		draw.mac cover txt1 txt2 txt3 appA appB bib
int.doc:	$(DOC)
		$(TBL) $(DOC) > $@

../int.doc:	int.doc
		mv int.doc ..

FLS =		README Makefile $(DOC)

.distr:		Makefile
		echo $(FLS) | tr ' ' '\012' >.distr

clean:
		rm -f int.doc

