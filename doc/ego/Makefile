REFS=-p refs.opt -p refs.stat -p refs.gen
INTRO=intro/intro?
OV=ov/ov?
IC=ic/ic?
CF=cf/cf?
IL=il/il?
SR=sr/sr?
CS=cs/cs?
SP=sp/sp?
UD=ud/ud?
LV=lv/lv?
CJ=cj/cj?
BO=bo/bo?
RA=ra/ra?
CA=ca/ca?
EGO=$(INTRO) $(OV) $(IC) $(CF) $(IL) $(SR) $(CS) $(SP) $(CJ) $(BO) \
    $(UD) $(LV) $(RA) $(CA)
REFER=refer
TROFF=troff
TBL=tbl
TARGET=-Tlp

../ego.doc:	refs.opt refs.stat refs.gen intro/head intro/tail $(EGO)
	 $(REFER) -sA+T -l4,2 $(REFS) intro/head $(EGO) intro/tail | $(TBL) > ../ego.doc

ego.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(EGO)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(EGO) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > ego.f
intro.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(INTRO)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(INTRO) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > intro.f
ov.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(OV)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(OV) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > ov.f
ic.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(IC)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(IC) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > ic.f
cf.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(CF)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(CF) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > cf.f
il.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(IL)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(IL) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > il.f
sr.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(SR)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(SR) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > sr.f
cs.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(CS)
	 $(REFER)	-sA+T -l4,2 $(REFS) intro/head $(CS) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > cs.f
sp.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(SP)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(SP) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > sp.f
cj.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(CJ)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(CJ) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > cj.f
bo.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(BO)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(BO) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > bo.f
ud.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(UD)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(UD) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > ud.f
lv.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(LV)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(LV) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > lv.f
ra.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(RA)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(RA) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > ra.f
ca.f:	refs.opt refs.stat refs.gen intro/head intro/tail $(CA)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(CA) intro/tail | $(TBL) | $(TROFF) $(TARGET) -ms > ca.f
