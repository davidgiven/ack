# $Header$

#PARAMS         do not remove this line!

REFER=refer
TBL=tbl

SRC_DIR = $(SRC_HOME)/doc/top

$(TARGET_HOME)/doc/top.doc:	$(SRC_DIR)/top.n $(SRC_DIR)/refs.top
	$(REFER) -sA+T -l4,2 -p $(SRC_DIR)/refs.top $(SRC_DIR)/top.n | $(TBL) > $@
