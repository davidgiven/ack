# $Header$

#PARAMS         do not remove this line!

SRC_DIR = $(SRC_HOME)/doc/LLgen

EQN=eqn
REFER=refer
TBL=tbl

$(TARGET_HOME)/doc/LLgen.doc:	$(SRC_DIR)/LLgen.n $(SRC_DIR)/LLgen.refs
	$(REFER) -sA+T -p $(SRC_DIR)/LLgen.refs $(SRC_DIR)/LLgen.n | $(EQN) | $(TBL) > $@
