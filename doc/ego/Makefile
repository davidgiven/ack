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

../ego.doc:	$(EGO)
	 $(REFER) -sA+T -l4,2 $(REFS) intro/head $(EGO) intro/tail > ../ego.doc

ego.f:	$(EGO)
	 $(REFER)  -sA+T -l4,2 $(REFS) intro/head $(EGO) intro/tail | nroff -ms > ego.f
intro.f:	$(INTRO)
	 $(REFER)  -sA+T -l4,2 $(REFS) ov/head $(INTRO) intro/tail | nroff -ms > intro.f
ov.f:	$(OV)
	 $(REFER)  -sA+T -l4,2 $(REFS) ov/head $(OV) intro/tail | nroff -ms > ov.f
ic.f:	$(IC)
	 $(REFER)  -sA+T -l4,2 $(REFS) ic/head $(IC) intro/tail | nroff -ms > ic.f
cf.f:	$(CF)
	 $(REFER)  -sA+T -l4,2 $(REFS) cf/head $(CF) intro/tail | nroff -ms > cf.f
il.f:	$(IL)
	 $(REFER)  -sA+T -l4,2 $(REFS) il/head $(IL) intro/tail | nroff -ms > il.f
sr.f:	$(SR)
	 $(REFER)  -sA+T -l4,2 $(REFS) sr/head $(SR) intro/tail | nroff -ms > sr.f
cs.f:	$(CS)
	 $(REFER)	-sA+T -l4,2 $(REFS) cs/head $(CS) intro/tail | nroff -ms > cs.f
sp.f:	$(SP)
	 $(REFER)  -sA+T -l4,2 $(REFS) sp/head $(SP) intro/tail | nroff -ms > sp.f
cj.f:	$(CJ)
	 $(REFER)  -sA+T -l4,2 $(REFS) cj/head $(CJ) intro/tail | nroff -ms > cj.f
bo.f:	$(BO)
	 $(REFER)  -sA+T -l4,2 $(REFS) bo/head $(BO) intro/tail | nroff -ms > bo.f
ud.f:	$(UD)
	 $(REFER)  -sA+T -l4,2 $(REFS) ud/head $(UD) intro/tail | nroff -ms > ud.f
lv.f:	$(LV)
	 $(REFER)  -sA+T -l4,2 $(REFS) lv/head $(LV) intro/tail | nroff -ms > lv.f
ra.f:	$(RA)
	 $(REFER)  -sA+T -l4,2 $(REFS) ra/head $(RA) intro/tail | nroff -ms > ra.f
ca.f:	$(CA)
	 $(REFER)  -sA+T -l4,2 $(REFS) ca/head $(CA) intro/tail | nroff -ms > ca.f
