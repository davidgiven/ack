# $Header$
d=../../..
h=$d/h

# Use apc -m.... for cross-compilation.
APC=apc
# The call to acc has to produce an executable file
# Add an -m parameter if needed.
ACC=acc

HEAD=$h/em_spec.h $h/em_pseu.h $h/em_mnem.h $h/em_mes.h $h/pc_size.h
LDFLAG=-i

all:            cpem

cpem.out:       cpem.m
		apc -mint --t -o cpem.out cpem.m

cpem:           cpem.m
		$(APC) $(LDFLAG) -o cpem cpem.m

# cpem.m is system dependent and may NOT be distributed
cpem.m:         cpem.p $(HEAD)
		-rm -f cpem.m
		-if $(APC) -I$h -DCHL -O -c.m cpem.p ; then :; else \
			$(ACC) -o move ../../pc/pem/move.c ; move ; rm -f move move.[oskm] ; \
		fi

clean:
		-rm -f *.[os] *.old

pr:
		@pr cpem.p

xref:
		xref cpem.p^pr -h "XREF PEM.P"

opr:
		make pr ^ opr
