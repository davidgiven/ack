# $Header$

#PARAMS         do not remove this line!

SRC_DIR = $(SRC_HOME)/doc/int
TBL=tbl

DOC =		$(SRC_DIR)/draw.mac \
		$(SRC_DIR)/cover \
		$(SRC_DIR)/txt1 \
		$(SRC_DIR)/txt2 \
		$(SRC_DIR)/txt3 \
		$(SRC_DIR)/appA \
		$(SRC_DIR)/appB \
		$(SRC_DIR)/bib

$(TARGET_HOME)/doc/int.doc:	$(DOC)
		$(TBL) $(DOC) > $@
