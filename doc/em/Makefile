head:   doc.pr

NROFF=nroff
TBL=tbl
FILES = macr.nr title.nr intro.nr mem.nr ispace.nr dspace.nr mapping.nr \
	types.nr descr.nr env.nr traps.nr mach.nr assem.nr \
	app.int.nr app.codes.nr app.exam.nr cont.nr
IOP=../../util/ass/ip_spec.t

doc.pr: $(FILES) itables em.i Makefile
	$(TBL) $(FILES) | $(NROFF) >doc.pr

distr:	$(FILES) itables em.i
	tbl $(FILES) | nroff -Tlp >doc.pr

opr:	doc.pr
	make pr | opr

pr:
	@make "NROFF="$NROFF doc.pr >makepr.out 2>&1
	@cat doc.pr

app.t:	itables em.i

em.i:	int/em.p
	@echo Sorry, this copy was edited by hand from int/em.p

itables: $(IOP) ip.awk
	awk -f ip.awk $(IOP) | sed 's/-/\\-/g" | $(TBL) >itables

.SUFFIXES : .pr .nr
.nr.pr: ; $(TBL) macr.nr $*.nr | $(NROFF) >$@

cont.t intro.t mem.t ispace.t dspace.t mapping.t succ.t descr.t iotrap.t mach.t assem.t kern.t app.t: macr.nr
