# $Id$

#PARAMS         do not remove this line!

SRC_DIR = $(SRC_HOME)/doc/sparc
TARGET = $(TARGET_HOME)/doc/sparc.doc

REFER=refer
TBL=tbl
PIC=pic
GRAP=grap

SRC =	$(SRC_DIR)/refs \
	$(SRC_DIR)/init \
	$(SRC_DIR)/title \
	$(SRC_DIR)/intro \
	$(SRC_DIR)/1 \
	$(SRC_DIR)/2 \
	$(SRC_DIR)/3 \
	4 \
	$(SRC_DIR)/5 \
	A \
	$(SRC_DIR)/B

$(TARGET):	$(SRC)
		$(REFER) -sA+T '-l\", ' -p $(SRC) | $(GRAP) | $(PIC) | $(TBL) > $@

4:	$(SRC_DIR)/4
	sed 's^pics/^$(SRC_DIR)/pics/^' < $(SRC_DIR)/4 > 4

A:	$(SRC_DIR)/A
	sed 's^pics/^$(SRC_DIR)/pics/^' < $(SRC_DIR)/A > A
