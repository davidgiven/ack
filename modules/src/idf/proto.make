# $Header$

#PARAMS		do not delete this line!

SRC_DIR = $(SRC_HOME)/modules/src/idf
MOD_DIR = $(TARGET_HOME)/modules

all:

install:	all
		cp $(SRC_DIR)/idf_pkg.body $(MOD_DIR)/pkg/idf_pkg.body
		cp $(SRC_DIR)/idf_pkg.spec $(MOD_DIR)/pkg/idf_pkg.spec
		cp $(SRC_DIR)/idf.3 $(MOD_DIR)/man/idf.3

cmp:		all
		-cmp $(SRC_DIR)/idf_pkg.body $(MOD_DIR)/pkg/idf_pkg.body
		-cmp $(SRC_DIR)/idf_pkg.spec $(MOD_DIR)/pkg/idf_pkg.spec
		-cmp $(SRC_DIR)/idf.3 $(MOD_DIR)/man/idf.3

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/idf_pkg.spec $(SRC_DIR)/idf_pkg.body

opr:
		make pr | opr

clean:
lintlib:
