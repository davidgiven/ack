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
	@make "NROFF="$(NROFF) TBL=$(TBL) doc.pr >makepr.out 2>&1
	@cat doc.pr

itables: $(IOP) ip.awk
	awk -f ip.awk $(IOP) | sed 's/-/\\-/g' | $(TBL) >itables

.SUFFIXES : .pr .nr
.nr.pr: ; $(TBL) macr.nr $*.nr | $(NROFF) >$@

clean:
	rm -f doc.pr itables
