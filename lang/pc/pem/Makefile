# $Header$
d=../../..
h=$d/h
PEM=$d/lib/pc_pem
PEM_OUT=$d/lib/pc_pem.out

HEAD=$h/em_spec.h $h/em_pseu.h $h/em_mnem.h $h/em_mes.h $h/pc_size.h
LDFLAG=-i

all:            pem pem.out

pem.out:        pem.m
		apc -mint --t -o pem.out pem.m

pem:            pem.m
		apc $(LDFLAG) -o pem pem.m

# pem.m is system dependent and may NOT be distributed
pem.m:          pem.p $(HEAD)
		-rm -f pem.m
		-if apc -I$h -O -c.m pem.p ; then :; else \
			acc -o move move.c ; move ; rm move move.[oskm] ; \
		fi

cmp:		pem
		cmp pem $(PEM)

install:	pem
		cp pem $(PEM)

distr:
		ln pem.p pem22.p ; apc -mpdp -c.m -I$h pem22.p ; rm pem22.p
		ln pem.p pem24.p ; apc -mvax2 -c.m -I$h pem24.p ; rm pem24.p
clean:
		-rm -f pem pem.out *.[os] *.old

pr:
		@pr pem.p

xref:
		xref pem.p^pr -h "XREF PEM.P"

opr:
		make pr ^ opr
