# $Header$

SUF=pr
PRINT=cat
NROFF=nroff
TBL=tbl
EQN=eqn
MS=-ms

RESFILES= \
	toolkit.$(SUF) install.$(SUF) ack.$(SUF) v7bugs.$(SUF) \
	peep.$(SUF) cg.$(SUF) ncg.$(SUF) regadd.$(SUF) LLgen.$(SUF) \
	basic.$(SUF) cref.$(SUF) pcref.$(SUF) val.$(SUF) \
	6500.$(SUF) i80.$(SUF) z80.$(SUF) top.$(SUF)

cref.$(SUF):        cref.doc
		$(TBL) $? | $(NROFF) >$@
v7bugs.$(SUF):      v7bugs.doc
		$(NROFF) $? >$@
ack.$(SUF):         ack.doc
		$(NROFF) $(MS) $? >$@
cg.$(SUF):		cg.doc
		$(NROFF) $(MS) $? >$@
ncg.$(SUF):		ncg.doc
		$(TBL) $? | $(NROFF) $(MS) >$@
regadd.$(SUF):		regadd.doc
		$(NROFF) $(MS) $? >$@
install.$(SUF):     install.doc
		$(TBL) $? | $(NROFF) $(MS) >$@
pcref.$(SUF):       pcref.doc
		$(NROFF) $? >$@
basic.$(SUF):       basic.doc
		$(NROFF) $(MS) $? >$@
peep.$(SUF):	peep.doc
		$(NROFF) $(MS) $? >$@
val.$(SUF):         val.doc
		$(NROFF) $? >$@
toolkit.$(SUF):	toolkit.doc
		$(NROFF) $(MS) $? >$@
LLgen.$(SUF):	LLgen/Makefile LLgen/LLgen.doc LLgen/LLgen.refs
		cd LLgen; make "NROFF="$(NROFF) "MS="$(MS) "EQN="$(EQN) \
			"TBL="$(TBL) "SUF="$(SUF)
6500.$(SUF):	6500.doc
		$(TBL) $? | $(NROFF) $(MS) >$@
i80.$(SUF):	i80.doc
		$(NROFF) $(MS) $? >$@
z80.$(SUF):	z80.doc
		$(NROFF) $(MS) $? >$@
top.$(SUF):	top/Makefile top/top.n top/refs.top
		cd top; make "NROFF="$(NROFF) "MS="$(MS) "EQN="$(EQN) \
			"TBL="$(TBL) "SUF="$(SUF)

install cmp:

distr:		install.doc
		tbl install.doc | nroff -Tlp $(MS) >install.pr

pr:
		@make "SUF="$(SUF) "NROFF="$(NROFF) \
			$(RESFILES) >make.pr.out 2>&1
		@$(PRINT) $(RESFILES)

opr:
		make pr | opr

clean:
		-rm -f *.old $(RESFILES) *.t *.out
