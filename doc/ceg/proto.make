# $Id$

#PARAMS         do not remove this line!

SRC_DIR = $(SRC_HOME)/doc/ceg

PIC=pic
TBL=tbl
REFER=refer

$(TARGET_HOME)/doc/ceg.doc:	$(SRC_DIR)/ceg.tr $(SRC_DIR)/ceg.ref
	$(PIC) $(SRC_DIR)/ceg.tr | $(REFER) -e -p $(SRC_DIR)/ceg.ref | $(TBL) > $@
