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

HEAD=$h/em_spec.h $h/em_pseu.h $h/em_mnem.h $h/em_mes.h $h/pc_size.h
LDFLAG=-i

all:            pem pem.out

pem.out:        pem.m
		apc -mint --t -o pem.out pem.m

pem:            pem.m
		$(APC) $(LDFLAG) -o pem pem.m

# pem.m is system dependent and may NOT be distributed
pem.m:          pem.p $(HEAD)
		-rm -f pem.m
		-if $(APC) -I$h -O -c.m pem.p ; then :; else \
			$(ACC) -o move move.c ; move ; rm -f move move.[oskm] ; \
		fi

cmp:		pem
		cmp pem $(PEM)

install:	pem
		cp pem $(PEM)

distr:
		rm -f pem22.p ; ln pem.p pem22.p
		apc -mpdp -c.m -I$h pem22.p ; rm -f pem22.p
		rm -f pem24.p ; ln pem.p pem24.p
		apc -mvax2 -c.m -I$h pem24.p ; rm -f pem24.p
clean:
		-rm -f pem pem.out *.[os] *.old

pr:
		@pr pem.p

xref:
		xref pem.p^pr -h "XREF PEM.P"

opr:
		make pr ^ opr
