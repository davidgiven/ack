# $Id$

#PARAMS		do not delete this line!

SRC_DIR = $(SRC_HOME)/modules/src/idf
MOD_DIR = $(TARGET_HOME)/modules

all:

install:	all
		-mkdir $(MOD_DIR)/pkg
		cp $(SRC_DIR)/idf_pkg.body $(MOD_DIR)/pkg/idf_pkg.body
		cp $(SRC_DIR)/idf_pkg.spec $(MOD_DIR)/pkg/idf_pkg.spec
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/idf.3 $(TARGET_HOME) ; \
		fi

cmp:		all
		-cmp $(SRC_DIR)/idf_pkg.body $(MOD_DIR)/pkg/idf_pkg.body
		-cmp $(SRC_DIR)/idf_pkg.spec $(MOD_DIR)/pkg/idf_pkg.spec

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/idf_pkg.spec $(SRC_DIR)/idf_pkg.body

opr:
		make pr | opr

clean:
lintlib:
