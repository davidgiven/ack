# $Id$

EQN=eqn
REFER=refer
TBL=tbl
TARGET=-Tlp

../LLgen.doc:	LLgen.n LLgen.refs
		$(REFER) -sA+T -p LLgen.refs LLgen.n | $(EQN) $(TARGET) | $(TBL) > $@
