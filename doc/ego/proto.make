# $Id$

#PARAMS         do not remove this line!

SRC_DIR = $(SRC_HOME)/doc/ego

REFS=-p $(SRC_DIR)/refs.opt -p $(SRC_DIR)/refs.stat -p $(SRC_DIR)/refs.gen
REFFILES = $(SRC_DIR)/refs.opt $(SRC_DIR)/refs.stat $(SRC_DIR)/refs.gen
INTRO=$(SRC_DIR)/intro/intro?
OV=$(SRC_DIR)/ov/ov?
IC=$(SRC_DIR)/ic/ic?
CF=$(SRC_DIR)/cf/cf?
IL=$(SRC_DIR)/il/il?
SR=$(SRC_DIR)/sr/sr?
CS=$(SRC_DIR)/cs/cs?
SP=$(SRC_DIR)/sp/sp?
UD=$(SRC_DIR)/ud/ud?
LV=$(SRC_DIR)/lv/lv?
CJ=$(SRC_DIR)/cj/cj?
BO=$(SRC_DIR)/bo/bo?
RA=$(SRC_DIR)/ra/ra?
CA=$(SRC_DIR)/ca/ca?
EGO=$(INTRO) $(OV) $(IC) $(CF) $(IL) $(SR) $(CS) $(SP) $(CJ) $(BO) \
    $(UD) $(LV) $(RA) $(CA)
REFER=refer
TROFF=troff
TBL=tbl
TARGET=-Tlp
HEAD = $(SRC_DIR)/intro/head
TAIL = $(SRC_DIR)/intro/tail

$(TARGET_HOME)/doc/ego.doc:	$(REFFILES) $(HEAD) $(TAIL) $(EGO)
	 $(REFER) -sA+T -l4,2 $(REFS) $(HEAD) $(EGO) $(TAIL) | $(TBL) > $(TARGET_HOME)/doc/ego.doc

ego.f:	$(REFFILES) $(HEAD) $(TAIL) $(EGO)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(EGO) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > ego.f
intro.f:	$(REFFILES) $(HEAD) $(TAIL) $(INTRO)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(INTRO) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > intro.f
ov.f:	$(REFFILES) $(HEAD) $(TAIL) $(OV)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(OV) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > ov.f
ic.f:	$(REFFILES) $(HEAD) $(TAIL) $(IC)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(IC) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > ic.f
cf.f:	$(REFFILES) $(HEAD) $(TAIL) $(CF)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(CF) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > cf.f
il.f:	$(REFFILES) $(HEAD) $(TAIL) $(IL)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(IL) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > il.f
sr.f:	$(REFFILES) $(HEAD) $(TAIL) $(SR)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(SR) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > sr.f
cs.f:	$(REFFILES) $(HEAD) $(TAIL) $(CS)
	 $(REFER)	-sA+T -l4,2 $(REFS) $(HEAD) $(CS) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > cs.f
sp.f:	$(REFFILES) $(HEAD) $(TAIL) $(SP)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(SP) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > sp.f
cj.f:	$(REFFILES) $(HEAD) $(TAIL) $(CJ)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(CJ) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > cj.f
bo.f:	$(REFFILES) $(HEAD) $(TAIL) $(BO)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(BO) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > bo.f
ud.f:	$(REFFILES) $(HEAD) $(TAIL) $(UD)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(UD) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > ud.f
lv.f:	$(REFFILES) $(HEAD) $(TAIL) $(LV)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(LV) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > lv.f
ra.f:	$(REFFILES) $(HEAD) $(TAIL) $(RA)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(RA) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > ra.f
ca.f:	$(REFFILES) $(HEAD) $(TAIL) $(CA)
	 $(REFER)  -sA+T -l4,2 $(REFS) $(HEAD) $(CA) $(TAIL) | $(TBL) | $(TROFF) $(TARGET) -ms > ca.f
