# $Header$

REFER=refer
TBL=tbl

../top.doc:	top.n refs.top
		$(REFER) -sA+T -l4,2 -p refs.top top.n | $(TBL) > $@
