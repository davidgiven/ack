# $Header$

MS=-ms
NROFF=nroff
REFER=refer
TBL=tbl
SUF=pr

../top.$(SUF):	top.n refs.top
		$(REFER) -sA+T -l4,2 -p refs.top top.n | $(TBL) | \
			$(NROFF) $(MS) > $@
