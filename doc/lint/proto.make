# $Header$

#PARAMS         do not remove this line!

SRC_DIR = $(SRC_HOME)/doc/lint

FP =	$(SRC_DIR)/frontpage

DOC =	$(SRC_DIR)/abstract \
	$(SRC_DIR)/contents \
	$(SRC_DIR)/chap1 \
	$(SRC_DIR)/chap2 \
	$(SRC_DIR)/chap3 \
	$(SRC_DIR)/chap4 \
	$(SRC_DIR)/chap5 \
	$(SRC_DIR)/chap6 \
	$(SRC_DIR)/chap7 \
	$(SRC_DIR)/chap8 \
	$(SRC_DIR)/chap9 \
	$(SRC_DIR)/appendix_A \
	$(SRC_DIR)/appendix_B

$(TARGET_HOME)/doc/lint.doc:	$(FP) $(DOC)
	cat $(FP) $(DOC) > $(TARGET_HOME)/doc/lint.doc
