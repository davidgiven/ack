# $Id$

#PARAMS		do not delete this line!

SRC_DIR = $(SRC_HOME)/modules/h
MOD_DIR = $(TARGET_HOME)/modules
FILES = \
	$(SRC_DIR)/ansi.h \
	$(SRC_DIR)/em.h \
	$(SRC_DIR)/em_arith.h \
	$(SRC_DIR)/em_code.h \
	$(SRC_DIR)/em_label.h \
	$(SRC_DIR)/em_codeCE.h \
	$(SRC_DIR)/emO_code.h \
	$(SRC_DIR)/em_codeO.h \
	$(SRC_DIR)/em_mesX.h

all:

install:	all
		-mkdir $(MOD_DIR)/h
		cp $(FILES) $(MOD_DIR)/h

cmp:		all

pr:
		@pr $(SRC_DIR)/proto.make $(FILES)

opr:
		make pr | opr

clean:
lintlib:
