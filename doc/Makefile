# $Header$

SUF=pr
PRINT=cat
NROFF=nroff
TBL=tbl
EQN=eqn
PIC=pic
REFER=refer
MS=-ms
TARGET=-Tlp

RESFILES= \
	toolkit.$(SUF) install.$(SUF) em.$(SUF) ack.$(SUF) v7bugs.$(SUF) \
	peep.$(SUF) cg.$(SUF) ncg.$(SUF) regadd.$(SUF) LLgen.$(SUF) \
	basic.$(SUF) crefman.$(SUF) pcref.$(SUF) val.$(SUF) ansi_C.$(SUF) \
	6500.$(SUF) i80.$(SUF) z80.$(SUF) top.$(SUF) ego.$(SUF) \
	m68020.$(SUF) occam.$(SUF) m2ref.$(SUF) ceg.$(SUF) nopt.$(SUF)

.SUFFIXES: .doc .$(SUF)

.doc.$(SUF):
		$(NROFF) $(MS) $(TARGET) $< > $@

crefman.$(SUF):	crefman.doc
		$(EQN) $(TARGET) crefman.doc | $(NROFF) $(MS) $(TARGET) >$@
ansi_C.$(SUF):	ansi_C.doc
		$(TBL) ansi_C.doc | $(NROFF) $(MS) $(TARGET) >$@
v7bugs.$(SUF):	v7bugs.doc
		$(NROFF) $(TARGET) v7bugs.doc >$@
install.$(SUF):	install.doc
		$(TBL) install.doc | $(NROFF) $(MS) $(TARGET) >$@
pcref.$(SUF):	pcref.doc
		$(TBL) pcref.doc | $(NROFF) $(TARGET) >$@
val.$(SUF):	val.doc
		$(NROFF) $(TARGET) val.doc >$@
6500.$(SUF):	6500.doc
		$(TBL) 6500.doc | $(NROFF) $(MS) $(TARGET) >$@
m68020.$(SUF):	m68020.doc
		$(EQN) $(TARGET) m68020.doc | $(TBL) | $(NROFF) $(MS) $(TARGET) >$@
ncg.$(SUF):	ncg.doc
		$(TBL) ncg.doc | $(NROFF) $(MS) $(TARGET) >$@
LLgen.doc:	LLgen.X
LLgen.X:
		cd LLgen; make "EQN="$(EQN) "TBL="$(TBL) "REFER="$(REFER) "TARGET="$(TARGET)
lint.doc:	lint.X
lint.X:
		cd lint; make
top.doc:	top.X
top.X:
		cd top; make "EQN="$(EQN) "TBL="$(TBL) "REFER="$(REFER) "TARGET="$(TARGET)
occam.doc:	occam.X
occam.X:
		cd occam; make "PIC="$(PIC) "TBL="$(TBL) "EQN="$(EQN) "TARGET="$(TARGET)
sparc.doc:	sparc.X
sparc.X:
		cd sparc; make "PIC="$(PIC) "TBL="$(TBL) "TARGET="$(TARGET)
ego.doc:	ego.X
ego.X:
		cd ego; make "REFER="$(REFER) "TARGET="$(TARGET) "TBL="$(TBL)
em.$(SUF):	em.X
em.X:
		cd em; make "TBL="$(TBL) "NROFF="$(NROFF) "SUF="$(SUF) "TARGET="$(TARGET)

int.doc:	int.X
int.X:
		cd int; make "TBL="$(TBL)

ceg.doc:	ceg.X
ceg.X:
		cd ceg; make "PIC="$(PIC) "TBL="$(TBL) "REFER="$(REFER) "TARGET="$(TARGET)

install cmp:

distr:		install.doc
		tbl install.doc | nroff -Tlpr $(MS) >install.pr

pr:
		@make "SUF="$(SUF) "NROFF="$(NROFF) "EQN="$(EQN) "TBL="$(TBL) \
			"PIC="$(PIC) "MS="$(MS) "TARGET="$(TARGET) \
			$(RESFILES) >make.pr.out 2>&1
		@$(PRINT) $(RESFILES)

opr:
		make pr | opr

clean:
		-rm -f *.old $(RESFILES) *.t *.out LLgen.doc top.doc \
			occam.doc ego.doc int.doc ceg.doc lint.doc sparc.doc
