# $Header$

EQN=eqn
REFER=refer
TBL=tbl

../LLgen.doc:	LLgen.n LLgen.refs
		$(REFER) -sA+T -p LLgen.refs LLgen.n | $(EQN) | $(TBL) > $@
