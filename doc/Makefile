# $Id$

# This Makefile is not supposed to be used in the doc source directory.
# Instead, it is supposed to be copied to the target doc directory.

SUF=dit
PRINT=dis
NROFF=troff
MS=-ms
OPR=dip

RESFILES= \
	toolkit.$(SUF) install.$(SUF) em.$(SUF) ack.$(SUF) v7bugs.$(SUF) \
	peep.$(SUF) cg.$(SUF) ncg.$(SUF) regadd.$(SUF) LLgen.$(SUF) \
	basic.$(SUF) crefman.$(SUF) pascal.$(SUF) pcref.$(SUF) val.$(SUF) \
	ansi_C.$(SUF) \
	6500.$(SUF) i80.$(SUF) z80.$(SUF) top.$(SUF) ego.$(SUF) \
	m68020.$(SUF) occam.$(SUF) m2ref.$(SUF) ceg.$(SUF) nopt.$(SUF) \
	sparc.$(SUF) int.$(SUF) lint.$(SUF)

.SUFFIXES: .doc .$(SUF) .lpr .out

.doc.$(SUF):
		$(NROFF) $(MS) $< > $@

# directly to the printer:
.doc.lpr:
		$(NROFF) $(MS) $< | $(OPR)

# to standard output
.doc.out:
		@$(NROFF) $(MS) $<

# Exceptions, to be run without -ms

v7bugs.$(SUF):	v7bugs.doc
		$(NROFF) v7bugs.doc >$@

v7bugs.lpr:	v7bugs.doc
		$(NROFF) v7bugs.doc | $(OPR)

v7bugs.out:	v7bugs.doc
		@$(NROFF) v7bugs.doc

pcref.$(SUF):	pcref.doc
		$(NROFF) pcref.doc >$@

pcref.lpr:	pcref.doc
		$(NROFF) pcref.doc | $(OPR)

pcref.out:	pcref.doc
		@$(NROFF) pcref.doc

val.$(SUF):	val.doc
		$(NROFF) val.doc >$@

val.lpr:	val.doc
		$(NROFF) val.doc | $(OPR)

val.out:	val.doc
		@$(NROFF) val.doc

pr:
		@make "SUF="$(SUF) "NROFF="$(NROFF) "MS="$(MS) \
			$(RESFILES) >make.pr.out 2>&1
		@$(PRINT) $(RESFILES)

# The 'opr' entry creates a lot of paper ... but the user must be able
# to write the doc directory. I hope that this limits the users of
# this entry to persons that know what they are doing.
opr:
		@make "SUF="$(SUF) "NROFF="$(NROFF) "MS="$(MS) $(RESFILES) 
		$(OPR) $(RESFILES)

clean:
		-rm -f $(RESFILES)

# The distr entry is only used when making a distribution tree.
# It makes a version of the installation manual, suitable for a simple
# line printer.
distr:		install.doc
		tbl install.doc | nroff -Tlpr $(MS) >install.pr
