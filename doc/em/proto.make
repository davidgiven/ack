# $Header$

#PARAMS         do not remove this line!

TBL=tbl

TARGET_DIR = $(TARGET_HOME)/doc

head:   $(TARGET_DIR)/em.doc

FILES = $(SRC_DIR)/macr.nr \
	$(SRC_DIR)/title.nr \
	$(SRC_DIR)/intro.nr \
	$(SRC_DIR)/mem.nr \
	$(SRC_DIR)/ispace.nr \
	$(SRC_DIR)/dspace.nr \
	$(SRC_DIR)/mapping.nr \
	$(SRC_DIR)/types.nr \
	$(SRC_DIR)/descr.nr \
	$(SRC_DIR)/env.nr \
	$(SRC_DIR)/traps.nr \
	$(SRC_DIR)/mach.nr \
	$(SRC_DIR)/assem.nr \
	$(SRC_DIR)/em.i \
	$(SRC_DIR)/app.codes.nr \
	$(SRC_DIR)/app.exam.nr \
	$(SRC_DIR)/cont.nr

IOP=$(SRC_HOME)/etc/ip_spec.t#			# to construct itables from

$(TARGET_DIR)/em.doc:	$(FILES) itables dispatdummy
	$(TBL) $(FILES) > $(TARGET_DIR)/em.doc

itables: $(IOP) $(SRC_DIR)/ip.awk
	awk -f $(SRC_DIR)/ip.awk $(IOP) | sed 's/-/\\-/g' | $(TBL) >itables

dispatdummy:	$(IOP) mkdispatch
	mkdispatch < $(IOP) > dispatdummy
	sed -f $(SRC_DIR)/dispat1.sed < dispatdummy | $(TBL) > dispat1
	sed -f $(SRC_DIR)/dispat2.sed < dispatdummy | $(TBL) > dispat2
	sed -f $(SRC_DIR)/dispat3.sed < dispatdummy | $(TBL) > dispat3

mkdispatch:	$(SRC_DIR)/mkdispatch.c
	$(UCC) -c -I$(UTIL_HOME)/h $(SRC_DIR)/mkdispatch.c
	$(UCC) $(ULDOPTIONS) -o mkdispatch mkdispatch.$(USUF) $(UTIL_HOME)/lib.bin/em_data.$(ULIBSUF)

clean:
	rm -f itables dispatdummy dispat? *.o mkdispatch Out
