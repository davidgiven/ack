SUF=pr
PRINT=cat
RESFILES=cref.$(SUF) pcref.$(SUF) val.$(SUF) v7bugs.$(SUF) install.$(SUF)\
ack.$(SUF) cg.$(SUF) regadd.$(SUF) peep.$(SUF) toolkit.$(SUF)
NROFF=nroff

cref.$(SUF):        cref.doc
		tbl $? | $(NROFF) >$@
v7bugs.$(SUF):      v7bugs.doc
		$(NROFF) -ms $? >$@
ack.$(SUF):         ack.doc
		$(NROFF) -ms $? >$@
cg.$(SUF):		cg.doc
		$(NROFF) -ms $? >$@
regadd.$(SUF):		regadd.doc
		$(NROFF) -ms $? >$@
install.$(SUF):     install.doc
		$(NROFF) -ms $? >$@
pcref.$(SUF):       pcref.doc
		$(NROFF) $? >$@
peep.$(SUF):	peep.doc
		$(NROFF) -ms $? >$@
val.$(SUF):         val.doc
		$(NROFF) $? >$@
toolkit.$(SUF):	toolkit.doc
		$(NROFF) -ms $? >$@

install cmp:

pr:
		@make "SUF="$SUF "NROFF="$NROFF "PRINT="$PRINT $(RESFILES) \
			>make.pr.out 2>&1
		@$(PRINT) $(RESFILES)

opr:
		make pr | opr

clean:
		-rm -f *.old $(RESFILES) *.t
