# $Header$

MS=-ms
EQN=eqn
NROFF=nroff
REFER=refer
TBL=tbl
SUF=pr

../LLgen.$(SUF):	LLgen.doc LLgen.refs
		$(REFER) -sA+T -p LLgen.refs LLgen.doc | $(EQN) | $(TBL) | \
			$(NROFF) $(MS) > $@
