HOME=../..

TBL=tbl
NROFF=nroff
SUF=pr

head:   ../em.$(SUF)

FILES = macr.nr title.nr intro.nr mem.nr ispace.nr dspace.nr mapping.nr \
	types.nr descr.nr env.nr traps.nr mach.nr assem.nr \
	app.int.nr app.codes.nr app.exam.nr cont.nr

IOP=$(HOME)/util/ass/ip_spec.t#			# to construct itables from

../em.$(SUF):	$(FILES) itables dispatdummy em.i Makefile
		$(TBL) $(FILES) | $(NROFF) > ../em.$(SUF)

app.codes.pr: app.codes.nr itables dispatdummy

itables: $(IOP) ip.awk
	awk -f ip.awk $(IOP) | sed 's/-/\\-/g' | $(TBL) >itables

dispatdummy:	$(IOP) mkdispatch
	mkdispatch < $(IOP) > dispatdummy
	sed -f dispat1.sed < dispatdummy | $(TBL) > dispat1
	sed -f dispat2.sed < dispatdummy | $(TBL) > dispat2
	sed -f dispat3.sed < dispatdummy | $(TBL) > dispat3

mkdispatch:	mkdispatch.c
	cc -I$(HOME)/util/ass -I$(HOME)/h -o mkdispatch mkdispatch.c $(HOME)/lib/em_data.a

.SUFFIXES : .pr .nr
.nr.pr: ; $(TBL) macr.nr $*.nr | $(NROFF) >$@

clean:
	rm -f *.pr itables *.out dispatdummy dispat? *.o mkdispatch
