# $Header$

SUF=pr
PRINT=cat
NROFF=nroff
TBL=tbl
EQN=eqn
PIC=pic
REFER=refer
MS=-ms

RESFILES= \
	toolkit.$(SUF) install.$(SUF) em.$(SUF) ack.$(SUF) v7bugs.$(SUF) \
	peep.$(SUF) cg.$(SUF) ncg.$(SUF) regadd.$(SUF) LLgen.$(SUF) \
	basic.$(SUF) crefman.$(SUF) pcref.$(SUF) val.$(SUF) \
	6500.$(SUF) i80.$(SUF) z80.$(SUF) top.$(SUF) ego.$(SUF) \
	m68020.$(SUF) occam.$(SUF) nopt.$(SUF)

.SUFFIXES: .doc .$(SUF)

.doc.$(SUF):
		$(NROFF) $(MS) $< > $@

crefman.$(SUF):	crefman.doc
		$(EQN) crefman.doc | $(NROFF) $(MS) >$@
v7bugs.$(SUF):	v7bugs.doc
		$(NROFF) v7bugs.doc >$@
install.$(SUF):	install.doc
		$(TBL) install.doc | $(NROFF) $(MS) >$@
pcref.$(SUF):	pcref.doc
		$(NROFF) pcref.doc >$@
val.$(SUF):	val.doc
		$(NROFF) val.doc >$@
6500.$(SUF):	6500.doc
		$(TBL) 6500.doc | $(NROFF) $(MS) >$@
LLgen.doc:	LLgen.X
LLgen.X:
		cd LLgen; make "EQN="$(EQN) "TBL="$(TBL) "REFER="$(REFER)
top.doc:	top.X
top.X:
		cd top; make "EQN="$(EQN) "TBL="$(TBL) "REFER="$(REFER)
occam.doc:	occam.X
occam.X:
		cd occam; make "PIC="$(PIC) "TBL="$(TBL) "EQN="$(EQN)
ego.doc:	ego.X
ego.X:
		cd ego; make "REFER="$(REFER)
em.$(SUF):	em.X
em.X:
		cd em; make "TBL="$(TBL) "NROFF="$(NROFF) "SUF="$(SUF)

install cmp:

distr:		install.doc
		tbl install.doc | nroff -Tlp $(MS) >install.pr

pr:
		@make "SUF="$(SUF) "NROFF="$(NROFF) "EQN="$(EQN) "TBL="$(TBL) \
			"PIC="$(PIC) "MS="$(MS) \
			$(RESFILES) >make.pr.out 2>&1
		@$(PRINT) $(RESFILES)

opr:
		make pr | opr

clean:
		-rm -f *.old $(RESFILES) *.t *.out LLgen.doc top.doc \
			occam.doc ego.doc
