# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ceg/util
CEGLIB = $(TARGET_HOME)/lib.bin/ceg
UTIL = $(CEGLIB)/util
UtilList = make_asobj make_own make_back make_ce make_ceg

all:

install:
	-mkdir $(CEGLIB)
	-mkdir $(UTIL)
	for i in $(UtilList) ; do cp $$i $(UTIL)/$$i ; done
	{ echo TARGET_HOME=$(TARGET_HOME); cat $(SRC_DIR)/install_ceg ; } > $(TARGET_HOME)/bin/install_ceg
	chmod +x $(TARGET_HOME)/bin/install_ceg
	{ echo TARGET_HOME=$(TARGET_HOME); echo SUF=$(SUF); echo LIBSUF=$(LIBSUF); cat $(SRC_DIR)/update_ceg ; } > $(TARGET_HOME)/bin/update_ceg
	chmod +x $(TARGET_HOME)/bin/update_ceg

cmp:
	-for i in $(UtilList) ; do cmp $$i $(UTIL)/$$i ; done

clean:

pr:
	@for i in $(UtilList) ; do pr $(SRC_DIR)/$$i ; done

opr:
	make pr | opr
