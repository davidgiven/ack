# $Id$

GRAP=grap
PIC=pic
EQN=eqn
REFER=refer
TBL=tbl

all:		../LLgen.doc ../LLgen_NCER.doc

../LLgen.doc:	LLgen.n LLgen.refs
		$(REFER) -sA+T -p LLgen.refs LLgen.n | $(EQN) | $(TBL) > $@

../LLgen_NCER.doc:	LLgen_NCER.n
		$(GRAP) LLgen_NCER.n | pic | eqn > $@
