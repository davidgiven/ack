# $Header$

REFER=refer
TBL=tbl
TARGET=-Tlp
PIC=pic
GRAP=grap

../sparc.doc:	refs title intro 1 2 3 4 5 A B init
		$(REFER) -sA+T '-l\", ' -p refs title intro 1 2 3 4 5 A B | $(GRAP) | $(PIC) | $(TBL) | soelim > $@
