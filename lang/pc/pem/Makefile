# $Header$
d=../../..
h=$d/h

# Use apc -m.... for cross-compilation.
APC=apc
# The call to acc has to produce an executable file
# Add an -m parameter if needed.
ACC=acc

PEM=$d/lib/pc_pem
PEM_OUT=$d/lib/pc_pem.out
MAN=$d/man/em_pem.6

HEAD=$h/em_spec.h $h/em_pseu.h $h/em_mnem.h $h/em_mes.h $h/pc_size.h
LDFLAG=-i

all:            pem pem.out

pem.out:        pem.m
		apc -mint --t -o pem.out pem.m

pem:            pem.m
		$(APC) $(LDFLAG) -O -o pem pem.m

# pem.m is system dependent and may NOT be distributed
pem.m:          pem.p $(HEAD)
		-rm -f pem.m
		-if $(APC) -I$h -O -c.m pem.p ; then :; else \
			$(ACC) -o move move.c ; move ; rm -f move move.[oskm] ; \
		fi

cmp:		pem
		-cmp pem $(PEM)
		-cmp em_pem.6 $(MAN)

install:	pem
		rm -f $(PEM)
		cp pem $(PEM)
		rm -f $(MAN)
		cp em_pem.6 $(MAN)

distr:
		rm -f pem22.[mp] ; ln pem.p pem22.p
		apc -mpdp -c.m -I$h pem22.p ; rm -f pem22.p
		rm -f pem24.[mp] ; ln pem.p pem24.p
		apc -mm68k2 -c.m -I$h pem24.p ; rm -f pem24.p
		rm -f pem44.[mp] ; ln pem.p pem44.p
		apc -mm68k4 -c.m -I$h pem44.p ; rm -f pem44.p

clean:
		-rm -f pem pem.out *.[os] *.old

pr:
		@pr pem.p

xref:
		xref pem.p^pr -h "XREF PEM.P"

opr:
		make pr ^ opr
