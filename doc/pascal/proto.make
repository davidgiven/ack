# $Id$

#PARAMS         do not remove this line!

SRC_DIR = $(SRC_HOME)/doc/pascal

PIC = pic

SRC =  \
	$(SRC_DIR)/ab+intro.doc \
	$(SRC_DIR)/internal.doc \
	$(SRC_DIR)/transpem.doc \
	$(SRC_DIR)/conf.doc \
	$(SRC_DIR)/options.doc \
	$(SRC_DIR)/extensions.doc \
	$(SRC_DIR)/deviations.doc \
	$(SRC_DIR)/hints.doc \
	$(SRC_DIR)/test.doc \
	$(SRC_DIR)/compar.doc \
	$(SRC_DIR)/improv.doc \
	$(SRC_DIR)/his.doc \
	$(SRC_DIR)/reference.doc \
	$(SRC_DIR)/syntax.doc \
	$(SRC_DIR)/rtl.doc \
	$(SRC_DIR)/example.doc

$(TARGET_HOME)/doc/pascal.doc:	$(SRC)
	$(PIC) $(SRC) > $$
