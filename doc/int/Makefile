# $Header$

TBL=/usr/ditroff/tbl

DOC =		draw.mac cover txt1 txt2 txt3 appA appB bib
int.doc:	$(DOC)
		$(TBL) $(DOC) > $@

FLS =		README .distr Makefile int.1 $(DOC)

.distr:		Makefile
		echo $(FLS) | tr ' ' '\012' >.distr

clean:
		rm -f int.doc

