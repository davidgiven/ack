# $Header$

NROFF=nroff
MANDIR=/usr/man

all:
	for i in *.[1-8] ; do \
		$(NROFF) -man head $$i > $$i.opr ; \
	done

install:
	for i in *.[1-8] ; do \
		num=expr $$i : '.*\.\([1-8]\)' ; \
		if test -f $(MANDIR)/man$$num/$$i ; \
			then cp $$i $(MANDIR)/man$$num/em_$$i ; \
		else cp $$i $(MANDIR)/man$$num/$$i ; \
		fi ; \
	done

opr:
	make pr | opr

pr:
	@make all >make.pr.out 2>&1 &
	@cat *.opr

clean:
	-rm -f *.opr *.out
