# $Id$

#PARAMS         do not remove this line!

TBL=tbl
EQN=eqn
PIC=pic
REFER=refer
GRAP=grap

RESFILES= \
	$(TARGET_HOME)/doc/toolkit.doc \
	$(TARGET_HOME)/doc/install.doc \
	$(TARGET_HOME)/doc/em.doc \
	$(TARGET_HOME)/doc/ack.doc \
	$(TARGET_HOME)/doc/v7bugs.doc \
	$(TARGET_HOME)/doc/peep.doc \
	$(TARGET_HOME)/doc/cg.doc \
	$(TARGET_HOME)/doc/ncg.doc \
	$(TARGET_HOME)/doc/regadd.doc \
	$(TARGET_HOME)/doc/LLgen.doc \
	$(TARGET_HOME)/doc/LLgen_NCER.doc \
	$(TARGET_HOME)/doc/pascal.doc \
	$(TARGET_HOME)/doc/basic.doc \
	$(TARGET_HOME)/doc/crefman.doc \
	$(TARGET_HOME)/doc/ansi_C.doc \
	$(TARGET_HOME)/doc/pcref.doc \
	$(TARGET_HOME)/doc/val.doc \
	$(TARGET_HOME)/doc/6500.doc \
	$(TARGET_HOME)/doc/i80.doc \
	$(TARGET_HOME)/doc/z80.doc \
	$(TARGET_HOME)/doc/m68020.doc \
	$(TARGET_HOME)/doc/sparc.doc \
	$(TARGET_HOME)/doc/top.doc \
	$(TARGET_HOME)/doc/ego.doc \
	$(TARGET_HOME)/doc/occam.doc \
	$(TARGET_HOME)/doc/m2ref.doc \
	$(TARGET_HOME)/doc/ceg.doc \
	$(TARGET_HOME)/doc/nopt.doc \
	$(TARGET_HOME)/doc/int.doc \
	$(TARGET_HOME)/doc/lint.doc \
	$(TARGET_HOME)/doc/install.pr \
	$(TARGET_HOME)/doc/READ_ME \
	$(TARGET_HOME)/doc/Makefile

install:	$(RESFILES)

$(TARGET_HOME)/doc/toolkit.doc:	$(SRC_HOME)/doc/toolkit.doc
		cat $(SRC_HOME)/doc/toolkit.doc >$@

$(TARGET_HOME)/doc/install.doc:	$(SRC_HOME)/doc/install.doc
		$(TBL) $(SRC_HOME)/doc/install.doc >$@

$(TARGET_HOME)/doc/em.doc:	em.X
em.X:
		cd em; make "TBL="$(TBL)

$(TARGET_HOME)/doc/ack.doc:	$(SRC_HOME)/doc/ack.doc
		cat $(SRC_HOME)/doc/ack.doc >$@

$(TARGET_HOME)/doc/v7bugs.doc:	$(SRC_HOME)/doc/v7bugs.doc
		cat $(SRC_HOME)/doc/v7bugs.doc >$@

$(TARGET_HOME)/doc/peep.doc:	$(SRC_HOME)/doc/peep.doc
		cat $(SRC_HOME)/doc/peep.doc >$@

$(TARGET_HOME)/doc/cg.doc:	$(SRC_HOME)/doc/cg.doc
		cat $(SRC_HOME)/doc/cg.doc >$@

$(TARGET_HOME)/doc/ncg.doc:	$(SRC_HOME)/doc/ncg.doc
		$(TBL) $(SRC_HOME)/doc/ncg.doc >$@

$(TARGET_HOME)/doc/regadd.doc:	$(SRC_HOME)/doc/regadd.doc
		cat $(SRC_HOME)/doc/regadd.doc >$@

$(TARGET_HOME)/doc/LLgen.doc:	LLgen.X
LLgen.X:
		cd LLgen; make "EQN="$(EQN) "TBL="$(TBL) "REFER="$(REFER) "GRAP="$(GRAP)

$(TARGET_HOME)/doc/LLgen_NCER.doc:	LLgen_NCER.X
LLgen_NCER.X:
		cd LLgen; make "EQN="$(EQN) "TBL="$(TBL) "REFER="$(REFER) "GRAP="$(GRAP)

$(TARGET_HOME)/doc/basic.doc:	$(SRC_HOME)/doc/basic.doc
		cat $(SRC_HOME)/doc/basic.doc >$@

$(TARGET_HOME)/doc/crefman.doc:	$(SRC_HOME)/doc/crefman.doc
		$(EQN) $(SRC_HOME)/doc/crefman.doc >$@

$(TARGET_HOME)/doc/ansi_C.doc:	$(SRC_HOME)/doc/ansi_C.doc
		$(TBL) $(SRC_HOME)/doc/ansi_C.doc >$@

$(TARGET_HOME)/doc/pcref.doc:	$(SRC_HOME)/doc/pcref.doc
		$(TBL) $(SRC_HOME)/doc/pcref.doc >$@

$(TARGET_HOME)/doc/val.doc:	$(SRC_HOME)/doc/val.doc
		cat $(SRC_HOME)/doc/val.doc >$@

$(TARGET_HOME)/doc/6500.doc:	$(SRC_HOME)/doc/6500.doc
		$(TBL) $(SRC_HOME)/doc/6500.doc >$@

$(TARGET_HOME)/doc/i80.doc:	$(SRC_HOME)/doc/i80.doc
		cat $(SRC_HOME)/doc/i80.doc >$@

$(TARGET_HOME)/doc/z80.doc:	$(SRC_HOME)/doc/z80.doc
		cat $(SRC_HOME)/doc/z80.doc >$@

$(TARGET_HOME)/doc/m68020.doc:	$(SRC_HOME)/doc/m68020.doc
		$(EQN) $(SRC_HOME)/doc/m68020.doc | $(TBL) >$@

$(TARGET_HOME)/doc/sparc.doc:	sparc.X
sparc.X:
		cd sparc; make "PIC="$(PIC) "TBL="$(TBL)

$(TARGET_HOME)/doc/pascal.doc:	pascal.X
pascal.X:
		cd pascal; make "PIC="$(PIC)

$(TARGET_HOME)/doc/top.doc:	top.X
top.X:
		cd top; make "EQN="$(EQN) "TBL="$(TBL) "REFER="$(REFER)

$(TARGET_HOME)/doc/ego.doc:	ego.X
ego.X:
		cd ego; make "REFER="$(REFER) "TBL="$(TBL)

$(TARGET_HOME)/doc/occam.doc:	occam.X
occam.X:
		cd occam; make "PIC="$(PIC) "TBL="$(TBL) "EQN="$(EQN)

$(TARGET_HOME)/doc/m2ref.doc:	$(SRC_HOME)/doc/m2ref.doc
		cat $(SRC_HOME)/doc/m2ref.doc >$@

$(TARGET_HOME)/doc/ceg.doc:	ceg.X
ceg.X:
		cd ceg; make "PIC="$(PIC) "TBL="$(TBL) "REFER="$(REFER) 

$(TARGET_HOME)/doc/nopt.doc:	$(SRC_HOME)/doc/nopt.doc
		cat $(SRC_HOME)/doc/nopt.doc >$@

$(TARGET_HOME)/doc/int.doc:	int.X
int.X:
		cd int; make "TBL="$(TBL)

$(TARGET_HOME)/doc/lint.doc:	lint.X
lint.X:
		cd lint; make

$(TARGET_HOME)/doc/install.pr:	$(SRC_HOME)/doc/install.pr
		cat $(SRC_HOME)/doc/install.pr >$@

$(TARGET_HOME)/doc/READ_ME:	$(SRC_HOME)/doc/READ_ME
		cat $(SRC_HOME)/doc/READ_ME >$@

$(TARGET_HOME)/doc/Makefile:	$(SRC_HOME)/doc/Makefile
		cat $(SRC_HOME)/doc/Makefile >$@

cmp:

clean:
		-rm -f *.old Out
