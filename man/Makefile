# $Header$

NROFF=nroff
MANDIR=/usr/man
SUF=opr

all:
	for i in *.[1-8] ; do \
		$(NROFF) -man head $$i > $$i.$(SUF) ; \
	done

install:
	for i in *.[1-8] ; do \
		num=expr $$i : '.*\.\([1-8]\)' ; \
		if test -f $(MANDIR)/man$$num/$$i ; \
			then cat head $$i > $(MANDIR)/man$$num/em_$$i ; \
		else cat head $$i $(MANDIR)/man$$num/$$i ; \
		fi ; \
	done

opr:
	make pr | opr

pr:
	@make all >make.pr.out 2>&1 &
	@cat *.opr

clean:
	-rm -f *.opr *.out
	-rm -f *.$(SUF)
