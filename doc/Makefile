# $Header$

SUF=pr
PRINT=cat
RESFILES=cref.$(SUF) pcref.$(SUF) val.$(SUF) v7bugs.$(SUF) install.$(SUF)\
ack.$(SUF) cg.$(SUF) regadd.$(SUF) peep.$(SUF) toolkit.$(SUF) LLgen.$(SUF)\
basic.$(SUF)
NROFF=nroff
MS=-ms

cref.$(SUF):        cref.doc
		tbl $? | $(NROFF) >$@
v7bugs.$(SUF):      v7bugs.doc
		$(NROFF) $(MS) $? >$@
ack.$(SUF):         ack.doc
		$(NROFF) $(MS) $? >$@
cg.$(SUF):		cg.doc
		$(NROFF) $(MS) $? >$@
regadd.$(SUF):		regadd.doc
		$(NROFF) $(MS) $? >$@
install.$(SUF):     install.doc
		$(NROFF) $(MS) $? >$@
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
LLgen.$(SUF):	LLgen.doc
		eqn $? | $(NROFF) $(MS) >$@

install cmp:

pr:
		@make "SUF="$SUF "NROFF="$NROFF "PRINT="$PRINT $(RESFILES) \
			>make.pr.out 2>&1
		@$(PRINT) $(RESFILES)

opr:
		make pr | opr

clean:
		-rm -f *.old $(RESFILES) *.t
